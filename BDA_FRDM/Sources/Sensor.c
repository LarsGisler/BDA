/*
 * Measure.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#include "Platform.h"
#include "Cpu.h"
#include "Sensor.h"
#include "AD1.h"
#include "ST.h"
#include "Event.h"


uint16_t sensor_data[NUMBER_OF_PIXEL];
uint16_t sensor_data_ready[NUMBER_OF_PIXEL];
byte readingData_flag = 0;
byte newData_flag = 0;
int integrationTime_us = START_INTEGRATION_TIME;
uint8_t integrationTime_adaption = 1;


void SENSOR_loadDummyData(){
	for(int pix_index = 0; pix_index<NUMBER_OF_PIXEL; pix_index++){
		sensor_data_ready[pix_index] = pix_index*100;
	}
}

void SENSOR_CLK_interrupt(){
	if(SENSOR_measureIntegrationTime){
		SENSOR_startMeasurement();
		}
	if(readingData_flag){
		SENSOR_readSensor();
	}
}

uint8_t SENSOR_measureIntegrationTime(){
	static int integrationTime_cntr = 0;
		if(integrationTime_cntr != (integrationTime_us/CLK_TICK_US)){
			integrationTime_cntr++;
			return 0;
		}
		else {
			return 1;
		}
}

void measurePixel(uint8_t pix_index){
	//(void) AD1_Measure(TRUE);
	(void) AD1_GetValue16(&sensor_data[pix_index]);
}

void SENSOR_readSensor(){
	static uint8_t cntr=0;
	static uint8_t pix_index = 0;

	if(cntr == TICKS_FOR_VIDEO){
		measurePixel(pix_index);
		cntr=0;
		if(pix_index == (NUMBER_OF_PIXEL-1)){
			pix_index = 0;
		}
		else {pix_index++;}
	}
	else {cntr++;}
	if(cntr == 3){
		ST_SetVal();
	}
}

void SENSOR_startMeasurement(){
	ST_ClrVal();
	readingData_flag = 1;
}

void SENSOR_EOS_interrupt(){
	readingData_flag = 0;
	EVNT_SetEvent(EVNT_NEW_DATA_AVAILABLE);
}

void SENSOR_handleNewData(){
	for(int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++)
		sensor_data_ready[pix_index] = sensor_data[pix_index];
	newData_flag = 1;
}
