/*
 * Communication.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#include "Platform.h"
#include "Cpu.h"
#include "Sensor.h"
#include "Communication.h"
#include "USB1.h"
#include "Trigger.h"
#include "Led.h"
#include "Event.h"
#include "LEDred.h"
#include "TestPin.h"

xSemaphoreHandle sem_calibration = NULL;
xSemaphoreHandle sem_dataCommand = NULL;


extern uint16_t sensor_data[];
extern State actualState;
extern int integrationTime_us;
uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];

uint8_t header_buffer[BUFFER_SIZE];

static portTASK_FUNCTION(communication, pvParameters) {
	for (;;) {
		 if(FRTOS1_xSemaphoreTake(sem_calibration,0/portTICK_RATE_MS)==pdTRUE) {
				  COM_sendCalibrationACK();
			  }
		 if(FRTOS1_xSemaphoreTake(sem_dataCommand,0/portTICK_RATE_MS)==pdTRUE) {
			 if(FRTOS1_xSemaphoreTake(sem_dataAvailable,0)==pdTRUE) {
					  COM_sendSensorData();
				  }
			  }
		 FRTOS1_vTaskDelay(20 / portTICK_RATE_MS);
	}
}

void COM_readCommand(){
	CDC1_GetChar(&header_buffer[0]);
	CDC1_GetChar(&header_buffer[1]);

	header.command = (header_buffer[0] & COMMAND_MASK);
	header.acknowledge_on = (header_buffer[1] & SERVICE_MASK)>>3;
	header.repeat_cnt = (header_buffer[1] & REPEAT_MASK)>>4;
}


void COM_extractCommandInfo(){
	if (header.command != 0) {
		LED3_On(); TRG_SetTrigger(TRG_LED3_OFF, 100, LED3m_Off, NULL);
		switch(header.command){
			case SEND_DATA_COMMAND:
				//EVNT_SetEvent(EVNT_DATA_REQUEST);
				if(actualState == Waiting){
				actualState = Measuring;
				xSemaphoreGive(sem_dataCommand);
				}
#if !PL_HAS_SENSOR
				EVNT_SetEvent(EVNT_NEW_DATA);
#endif
				break;
			case CALIBRATION_COMMAND:
				//EVNT_SetEvent(EVNT_CALIBRATION);
				for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
						sensor_calibration_data[pix_index] = 0;
				}
				integrationTime_us = START_INTEGRATION_TIME;
				actualState = Calibrating;
#if !PL_HAS_SENSOR
				EVNT_SetEvent(EVNT_CALIBRATION_FINISHED);
#endif
				break;
			default: break;
		}
		header.command = 0;
		header_buffer[0] = 0;
		header_buffer[1] = 0;
	}

}

void COM_sendCalibrationACK(){
	uint16_t header = buildProtocolHeader(CALIBRATION_ACK_COMMAND);
	uint8_t headerL = header;
	uint8_t headerH = header>>8;
	CDC1_SendChar((char)headerL);
	CDC1_SendChar((char)headerH);
	CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer));
	LEDred_ClrVal();
}

void COM_sendPixel(uint8_t pix_index){
	uint8_t byteL = sensor_data[pix_index];
	uint8_t byteH = sensor_data[pix_index]>>8;
	CDC1_SendChar((char)byteL);
	CDC1_SendChar((char)byteH);
	CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer));
}

void COM_sendSensorData(){
	//TestPin_SetVal();
	for(int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++){
		COM_sendPixel(pix_index);
	}
	COM_sendMeasuredDataEnd();
}

void COM_sendMeasuredDataEnd(void){
	uint8_t byteL = MEASURED_DATA_END;
	uint8_t byteH = MEASURED_DATA_END>>8;
	CDC1_SendChar((char)byteL);
	CDC1_SendChar((char)byteH);
	CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer));
	//TestPin_ClrVal();
}

uint16_t buildProtocolHeader(uint8_t command){
	uint16_t header = 0;
	uint8_t headerL = command;
	uint8_t headerH = 0;
#if PL_HAS_SERVICE_PROTOCOL
	uint8_t service = 0b00001000;
	headerH = (headerH | service);
#endif
#if PL_HAS_REPEAT_PROTOCOL
	uint8_t repeat = 0b00110000;
	headerH = (headerH | repeat);
#endif
	header = headerL | (headerH<<8);
	return header;

}

void COM_Init(){
	FRTOS1_vSemaphoreCreateBinary(sem_calibration);
	if (sem_calibration == NULL) {
		for (;;) {}
	}
	FRTOS1_xSemaphoreTake(sem_calibration,0/portTICK_RATE_MS);
	FRTOS1_vSemaphoreCreateBinary(sem_dataCommand);
	if (sem_dataCommand == NULL) {
			for (;;) {}
		}
	FRTOS1_xSemaphoreTake(sem_dataCommand,0/portTICK_RATE_MS);
	if (FRTOS1_xTaskCreate(communication, "com task", configMINIMAL_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
			for (;;) {
			}
	}
}

void COM_Deinit(){

}




