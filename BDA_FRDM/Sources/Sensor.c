/*
 * Sensor.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars Gisler
 */

#include "Platform.h"
#include "Cpu.h"
#include "Sensor.h"
#include "Communication.h"
#include "AD1.h"
#include "ST.h"
#include "LEDred.h"
#include "EN.h"
#include "SHDN.h"
#include "Event.h"
#include "CS1.h"
#include "TU2.h"
#if !PL_HAS_SENSOR
#include "EventHandler.h"
#endif
#include "TestPin.h"

xSemaphoreHandle sem_dataAvailable = NULL;
xSemaphoreHandle sem_EOS = NULL;

byte readingData_flag = 0;
int integrationTime_us = START_INTEGRATION_TIME;
int integrationTime_cntr = 0;
uint8_t integrationTime_adaption = 1;

State actualState = Starting;

void SENSOR_loadDummyData() {
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sensor_data[pix_index] = pix_index * 100;
	}
}

static portTASK_FUNCTION(EOS_handler, pvParameters) {
	for (;;) {
		 if(xSemaphoreTake(sem_EOS,0/portTICK_RATE_MS)==pdTRUE) {
			 readingData_flag = 0;
			 if(actualState == Calibrating){
			 		SENSOR_handleCalibrationData();
			 	}
			 else{
			 		SENSOR_handleNewData();
			 }
		 }
		 //FRTOS1_vTaskDelay(10 / portTICK_RATE_MS);
	}
}


void SENSOR_CLK_interrupt() {
	if (actualState != Starting) {
		if (integrationTime_cntr >= (integrationTime_us / CLK_TICK_US)) {
			readingData_flag = 1;
			integrationTime_cntr = 0;
			pix_index = 0;
			clk_cntr = 3;
			ST_ClrVal();
		} else {
			integrationTime_cntr++;
		}
		if (readingData_flag) {
			if (clk_cntr == TICKS_FOR_VIDEO) {
				measurePixel(pix_index);
				clk_cntr = 1;
			}
			else {
				clk_cntr++;
			}
			if (clk_cntr == 6) {
				ST_SetVal();
			}
		}
	}
}

/*uint8_t SENSOR_measureIntegrationTime() {
	static int integrationTime_cntr = 0;
	if (integrationTime_cntr != (integrationTime_us / CLK_TICK_US)) {
		integrationTime_cntr++;
		return 0;
	} else {
		return 1;
	}
}*/

void SENSOR_Start() {
	//(void)TU2_Enable(NULL);
	actualState = Waiting;
}

void SENSOR_handleCalibrationData() {
	static uint8_t calibration_cntr = 0;
	if(calibration_cntr != 0){
		FRTOS1_taskENTER_CRITICAL();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
			sensor_calibration_data[pix_index] += sensor_data_raw[pix_index];
			sensor_data_raw[pix_index] = 0;
		}
		FRTOS1_taskEXIT_CRITICAL();
	}
	if(calibration_cntr == 3){
		FRTOS1_taskENTER_CRITICAL();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
				sensor_calibration_data[pix_index] = sensor_calibration_data[pix_index]/3;
		}
		FRTOS1_taskEXIT_CRITICAL();
		integrationTime_us = START_INTEGRATION_TIME;
		//EVNT_SetEvent(EVNT_CALIBRATION_FINISHED);
		xSemaphoreGive(sem_calibration);
		actualState = Waiting;
		calibration_cntr = 0;
	}
	else{
		calibration_cntr++;
		actualState = Calibrating;
	}
}

void measurePixel() {
	TestPin_SetVal();
	(void) AD1_Measure(FALSE);
}

/*void SENSOR_readSensor() {
	static uint8_t cntr = 0;
	static uint8_t pix_index = 0;

	if (cntr == TICKS_FOR_VIDEO) {
		measurePixel(pix_index);
		cntr = 0;
		if (pix_index == (NUMBER_OF_PIXEL - 1)) {
			pix_index = 0;
		} else {
			pix_index++;
		}
	} else {
		cntr++;
	}
	if (cntr == 3) {
		ST_SetVal();
	}
}*/

/*void SENSOR_startMeasurement() {
	ST_ClrVal();
	readingData_flag = 1;
	integrationTime_us = START_INTEGRATION_TIME;
}*/


void SENSOR_handleNewData() {
	uint8_t correction_factor = integrationTime_adaption;
	FRTOS1_taskENTER_CRITICAL();
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		int pixel_data = (sensor_data_raw[pix_index] - sensor_calibration_data[pix_index]);
		if((pixel_data < 0) || (pixel_data > 60000)){
			 sensor_data_raw[pix_index] = 0;
		}
		else{
			sensor_data[pix_index] = pixel_data;
		}
	}
	FRTOS1_taskEXIT_CRITICAL();
	if ((!adaptIntegrationTime()) && (actualState == Measuring)) {
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
			sensor_data[pix_index] = sensor_data[pix_index]/correction_factor;
		}
		//EVNT_SetEvent(EVNT_NEW_DATA);
		xSemaphoreGive(sem_dataAvailable);
		actualState = Waiting;
	}
}

void SENSOR_Init() {
	readingData_flag = 0;
	integrationTime_adaption = 1;
	pix_index = 0;
	LEDred_SetVal();
	EN_SetVal();
	SHDN_SetVal();
	integrationTime_us = START_INTEGRATION_TIME;
	FRTOS1_vSemaphoreCreateBinary(sem_dataAvailable);
		if (sem_dataAvailable == NULL) {
			for (;;) {}
		}
	FRTOS1_vSemaphoreCreateBinary(sem_EOS);
		if (sem_EOS == NULL) {
			for (;;) {}
		}
	if (FRTOS1_xTaskCreate(EOS_handler, "eos handler", configMINIMAL_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
			for (;;) {
			}
		}
#if !PL_HAS_SENSOR
	SENSOR_loadDummyData();
#endif
	actualState = Starting;
}

void SENSOR_Deinit() {

}


byte adaptIntegrationTime() {
	uint8_t adapt_flag = 0;
	int static peak_timeout = 10;
	int static peak_detected = 0;
	int pixel_avg = getPixelAvg();
	int nbrOfPeaks =  getNbrOfPeaks();

	if((nbrOfPeaks >= 2) && (pixel_avg < (0.4*MAX_PIX_VALUE_CALIBRATED))){
		peak_detected = 1;
		peak_timeout = 40;
		if(integrationTime_us >= 800000){
			peak_timeout = 6;
		}
	}
	if(peak_detected && (peak_timeout != 0)){
			peak_timeout--;
	}
	if((pixel_avg < 0.05*MAX_PIX_VALUE_CALIBRATED) || (peak_timeout == 0)){
		peak_detected = 0;
	}

	if((pixel_avg >= 0.4*MAX_PIX_VALUE_CALIBRATED) || (nbrOfPeaks >= 2)){
		if(integrationTime_adaption != 1){
			integrationTime_us = integrationTime_us/2;
			integrationTime_adaption = integrationTime_adaption/2;
			adapt_flag = 1;
		}
	}
	if((pixel_avg < 0.2*MAX_PIX_VALUE_CALIBRATED) && (!peak_detected)){
		integrationTime_us = integrationTime_us*2;
		integrationTime_adaption = integrationTime_adaption*2;
		adapt_flag = 1;
	}
	// if the integrationTime exceeds 4480ms
	if(integrationTime_adaption >= 128){
		integrationTime_us = 128*START_INTEGRATION_TIME;
		integrationTime_adaption = 128;
		adapt_flag = 0;
	}
	// if the integrationTime is smaller than 50ms
	if(integrationTime_adaption <= 1){
		integrationTime_us = START_INTEGRATION_TIME;
		integrationTime_adaption = 1;
		adapt_flag = 0;
	}
	if(adapt_flag == 1){
		return 1;
	}
	else{
		return 0;
	}
}

int getPixelAvg() {
	int sum = 0;
	for(int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++){
		sum = sum + sensor_data[pix_index];
	}
	return sum/256;
}

int getNbrOfPeaks() {
	int peak_cntr = 0;
	//int peak_sum = 0;
	//int pix_avg = getPixelAvg();
	for(int pix_index=0; pix_index < NUMBER_OF_PIXEL; pix_index++){
			if(sensor_data[pix_index] >= MAX_PIX_VALUE_CALIBRATED){
				peak_cntr++;
				//peak_sum = peak_sum + sensor_data[pix_index];
			}
	}
	//if((peak_cntr > 5) && (peak_cntr < 50)){
		return peak_cntr; // there is a peak in the spectrum, return average value of peak pixels
	//}
	//else{
	//	return 0; // there is no peak in the spectrum, even distribution of pixels
	//}
}

