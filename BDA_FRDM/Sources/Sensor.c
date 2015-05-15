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

void SENSOR_CLK_interrupt() {
	if (actualState != Starting) {
		ST_SetVal();
		if (integrationTime_cntr >= (integrationTime_us / CLK_TICK_US)) {
			readingData_flag = 1;
			integrationTime_cntr = 0;
			pix_index = 0;
			clk_cntr = 1;
			ST_ClrVal();
		} else {
			integrationTime_cntr++;
		}
		if (readingData_flag) {
			if(clk_cntr == 2){
				measurePixel(pix_index);
				clk_cntr=1;
			}
			else{
				clk_cntr++;
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
		CS1_CriticalVariable();
		CS1_EnterCritical();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
			sensor_calibration_data[pix_index] += sensor_data_raw[pix_index];
			sensor_data_raw[pix_index] = 0;
		}
		CS1_ExitCritical();
	}
	if(calibration_cntr == 3){
		CS1_CriticalVariable();
		CS1_EnterCritical();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
				sensor_calibration_data[pix_index] = sensor_calibration_data[pix_index]/3;
		}
		CS1_ExitCritical();
		integrationTime_us = START_INTEGRATION_TIME;
		EVNT_SetEvent(EVNT_CALIBRATION_FINISHED);
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

void SENSOR_EOS_interrupt() {
	readingData_flag = 0;
	EVNT_SetEvent(EVNT_SENSOR_EOS);
}

void SENSOR_handleNewData() {
	uint8_t correction_factor = integrationTime_adaption;
	CS1_CriticalVariable();
	CS1_EnterCritical();
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		int pixel_data = (sensor_data_raw[pix_index] - sensor_calibration_data[pix_index]);
		if((pixel_data < 0) || (pixel_data > 60000)){
			 sensor_data_raw[pix_index] = 0;
		}
		else{
			sensor_data[pix_index] = pixel_data;
		}
	}
	CS1_ExitCritical();
	if ((!adaptIntegrationTime()) && (actualState == Measuring)) {
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
			sensor_data[pix_index] = sensor_data[pix_index]/correction_factor;
		}
		EVNT_SetEvent(EVNT_NEW_DATA);
		actualState = Waiting;
	}
}

void SENSOR_init() {
	readingData_flag = 0;
	integrationTime_adaption = 1;
	pix_index = 0;
	LEDred_SetVal();
	EN_SetVal();
	SHDN_SetVal();
	integrationTime_us = START_INTEGRATION_TIME;
#if !PL_HAS_SENSOR
	SENSOR_loadDummyData();
#endif
	actualState = Starting;
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
	// if the integrationTime exceeds 3200ms
	if(integrationTime_adaption >= 64){
		adapt_flag = 0;
		integrationTime_us = 3200000;
		integrationTime_adaption = 64;
	}
	// if the integrationTime is smaller than 50ms
	if(integrationTime_adaption <= 1){
		adapt_flag = 0;
		integrationTime_us = 50000;
		integrationTime_adaption = 1;
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

