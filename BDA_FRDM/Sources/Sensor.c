/*
 * Measure.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#include "Platform.h"
#include "Cpu.h"
#include "Sensor.h"
#include "Communication.h"
#include "AD1.h"
#include "ST.h"
#include "Event.h"
#include "CS1.h"
#include "TU2.h"
#if !PL_HAS_SENSOR
#include "EventHandler.h"
#endif

uint16_t sensor_data_raw[NUMBER_OF_PIXEL];
uint16_t sensor_data[NUMBER_OF_PIXEL];
uint16_t sensor_calibration_data[NUMBER_OF_PIXEL];
byte readingData_flag = 0;
byte newData_flag = 0;
byte calibrated_flag = 0;
int integrationTime_us = START_INTEGRATION_TIME;
uint8_t integrationTime_adaption = 1;

State actualState = Starting;

void SENSOR_loadDummyData() {
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sensor_data[pix_index] = pix_index * 100;
	}
}

void SENSOR_CLK_interrupt() {
	if (SENSOR_measureIntegrationTime) {
		SENSOR_startMeasurement();
	}
	if (readingData_flag) {
		SENSOR_readSensor();
	}
}

uint8_t SENSOR_measureIntegrationTime() {
	static int integrationTime_cntr = 0;
	if (integrationTime_cntr != (integrationTime_us / CLK_TICK_US)) {
		integrationTime_cntr++;
		return 0;
	} else {
		return 1;
	}
}

void SENSOR_Start() {
	//(void)TU2_Enable(NULL);
	actualState = Waiting;
}

void SENSOR_handleCalibrationData() {
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sensor_calibration_data[pix_index] = sensor_data_raw[pix_index];
		sensor_data_raw[pix_index] = 0;
	}
	integrationTime_us = START_INTEGRATION_TIME;
	EVNT_SetEvent(EVNT_CALIBRATION_FINISHED);
	actualState = Waiting;
}

void measurePixel(uint8_t pix_index) {
	//(void) AD1_Measure(TRUE);
	(void) AD1_GetValue16(&sensor_data_raw[pix_index]);
}

void SENSOR_readSensor() {
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
}

void SENSOR_startMeasurement() {
	ST_ClrVal();
	readingData_flag = 1;
	integrationTime_us = START_INTEGRATION_TIME;
}

void SENSOR_EOS_interrupt() {
	readingData_flag = 0;
	EVNT_SetEvent(EVNT_SENSOR_EOS);
}

void SENSOR_handleNewData() {
	CS1_CriticalVariable();
	CS1_EnterCritical();
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sensor_data[pix_index] = sensor_data_raw[pix_index];
		sensor_data_raw[pix_index] = 0;
	}
	CS1_ExitCritical();
	if (checkIntegrationTime()) {
		actualState = Measuring;
	} else {
		EVNT_SetEvent(EVNT_NEW_DATA);
		actualState = Waiting;
	}
}

void SENSOR_init() {
	readingData_flag = 0;
	newData_flag = 0;
	byte calibrated_flag = 0;
	integrationTime_adaption = 1;
#if !PL_HAS_SENSOR
	SENSOR_loadDummyData();
#endif
	actualState = Starting;
}

byte checkIntegrationTime() {
	int pixel_avg = getPixelAvg();
	int peak_avg = getPeakAvg();
	// write code here...

}
int getPixelAvg() {
	int sum = 0;
	for(uint8_t pix_index=0; pix_index < NUMBER_OF_PIXEL; pix_index++){
		sum = sum + sensor_data[pix_index];
	}
	return sum/256;
}

int getPeakAvg() {
	byte peak_cntr = 0;
	int peak_sum = 0;
	int pix_avg = getPixelAvg();
	for(uint8_t pix_index=0; pix_index < NUMBER_OF_PIXEL; pix_index++){
			if(sensor_data[pix_index] > (3*pix_avg)){
				peak_cntr++;
				peak_sum = peak_sum + sensor_data[pix_index];
			}
	}
	if((peak_cntr > 2) && (peak_cntr < 40)){
		return peak_sum/peak_cntr; // there is a peak in the spectrum, return average value of peak pixels
	}
	else{
		return 0; // there is no peak in the spectrum, even distribution of pixels
	}
}

