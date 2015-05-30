/**
 * \file
 * \brief Sensor Driver
 * \author Lars Gisler
 *
 * This module provides all functions to communicate with the Hamamatsu Sensor and to read and store the sensor data.
 * It ensures the synchronization between the A/D conversion and the sensor video signal. The modul furthermore provides
 * an algorithm to dynamically adapt the integration time.
 */

#include "Platform.h"
#include "Cpu.h"
#include "Sensor.h"
#include "Communication.h"
#include "AD1.h"
#include "ST.h"
#include "LEDred.h"
#include "EN.h"
#include "Event.h"
#include "CS1.h"
#include "TU2.h"
#include "Led.h"
#include "Trigger.h"
#if !PL_HAS_SENSOR
#include "EventHandler.h"
#endif
#include "TestPin.h"

xSemaphoreHandle sem_dataAvailable = NULL;
xSemaphoreHandle sem_EOS = NULL;

uint16_t sensor_data_raw[NUMBER_OF_PIXEL];
uint16_t sensor_data[NUMBER_OF_PIXEL];
uint16_t sensor_data_ready[NUMBER_OF_PIXEL];
uint16_t sensor_calibration_data[NUMBER_OF_PIXEL];
uint8_t pixel_index;

int integrationTime_us = START_INTEGRATION_TIME;
int integrationTime_cntr = 0;
/*! flag indicates whether the integration time is elapsed and the sensor data needs to be read */
byte readingData_flag = 0;
/*! Is used to adapt the sensor data in dependence on the integration time*/
uint8_t integrationTime_adaption = 1;
/* counter is used to synchronize the sensor video signal with the AD conversion*/
int8_t clk_cntr;

State actualState = Starting;

void SENSOR_loadDummyData() {
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sensor_data_ready[pix_index] = pix_index * 100;
	}
}

static portTASK_FUNCTION(EOS_Handler, pvParameters) {
	for (;;) {
		 if(xSemaphoreTake(sem_EOS,0/portTICK_RATE_MS)==pdTRUE) {
			 readingData_flag = 0;
			 /* action depends on program state */
			 if(actualState == Calibrating){
			 		SENSOR_handleCalibrationData();
			 	}
			 if(actualState == Measuring){
			 		SENSOR_handleNewData();
			 }
		 }
	}
}


void SENSOR_CLK_interrupt() {
	if (actualState != Starting) {
		if (integrationTime_cntr >= (integrationTime_us / CLK_TICK_US)) { /* if integration time is elapsed do...*/
			readingData_flag = 1;
			integrationTime_cntr = 0;
			pixel_index = 0;
			clk_cntr = 3; /* necessary to define the time between the start signal and the first AD conversion*/
			ST_ClrVal(); /* give start signal to the sensor*/
		} else {
			integrationTime_cntr++;
		}
		if (readingData_flag) {
			if (clk_cntr == TICKS_FOR_VIDEO) {
				measurePixel(pixel_index); /* read pixel value from sensor video signal */
				clk_cntr = 1;
			}
			else {
				clk_cntr++;
			}
			if (clk_cntr == 6) { /* put start signal back to high level after a certain number of clk edges*/
				ST_SetVal();
			}
		}
	}
}


void SENSOR_handleCalibrationData() {
	static uint8_t calibration_cntr = 0;
	if(calibration_cntr != 0){ /* ignore the first measure process because most pixels are saturated */
		CS1_CriticalVariable();
		CS1_EnterCritical();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) { /* sum up three complete measure processes */
			sensor_calibration_data[pix_index] += sensor_data_raw[pix_index];
			sensor_data_raw[pix_index] = 0;
		}
		CS1_ExitCritical();
	}
	if(calibration_cntr == 3){
		CS1_CriticalVariable();
		CS1_EnterCritical();
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) { /* calculate average of three measure processes */
				sensor_calibration_data[pix_index] = sensor_calibration_data[pix_index]/3;
		}
		CS1_ExitCritical();
		integrationTime_us = START_INTEGRATION_TIME;
		xSemaphoreGive(sem_calibration); /* release semaphore to indicate the calibration process has been finished */
		actualState = Measuring;
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


void SENSOR_handleNewData() {
	uint8_t correction_factor = integrationTime_adaption;
	CS1_CriticalVariable();
	CS1_EnterCritical();
	for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) { /* deduct the calibration data from the sensor data*/
		int pixel_data = (sensor_data_raw[pix_index] - sensor_calibration_data[pix_index]);
		if((pixel_data < 0) || (pixel_data > 60000)){ /* handle overflow, just in case calibration value is higher than the sensor value */
			 sensor_data[pix_index] = 0;
		}
		else{
			sensor_data[pix_index] = pixel_data;
		}
	}
	if ((!adaptIntegrationTime())) { /* enter only if the integration time stays the same */
		for (int pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
			sensor_data_ready[pix_index] = sensor_data[pix_index]/correction_factor; /* prepare data to send it over usb */
		} /* correction factor depends on integration time */
		xSemaphoreGive(sem_dataAvailable); /* release semaphore when new data is ready to be sent over USB*/
	}
	CS1_ExitCritical();
}

void SENSOR_Init() {
	readingData_flag = 0;
	integrationTime_adaption = 1;
	pixel_index = 0;
	LEDred_SetVal();
	EN_SetVal();
	integrationTime_us = START_INTEGRATION_TIME;
	FRTOS1_vSemaphoreCreateBinary(sem_dataAvailable);
		if (sem_dataAvailable == NULL) {
			for (;;) {} /* error */
		}
	FRTOS1_xSemaphoreTake(sem_dataAvailable,10/portTICK_RATE_MS); /* take semaphore to set it to zero*/
	FRTOS1_vSemaphoreCreateBinary(sem_EOS);
		if (sem_EOS == NULL) {
			for (;;) {} /* error */
		}
	FRTOS1_xSemaphoreTake(sem_EOS,10/portTICK_RATE_MS); /* take semaphore to set it to zero*/
	if (FRTOS1_xTaskCreate(EOS_Handler, "eos Handler", configMINIMAL_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
			for (;;) {} /* error */
		}
#if !PL_HAS_SENSOR
	SENSOR_loadDummyData(); /* load dummy data when sensor is inactive in Platform.h */
#endif
	actualState = Starting;
}

void SENSOR_Deinit() {
}


byte adaptIntegrationTime() {
	uint8_t adapt_flag = 0; /* holds decision whether integration time changed or not */
	int static peak_timeout = 10;  /* peak timeout to avoid fast changing/toggling of between two integration times*/
	int static peak_detected = 0;
	int pixel_avg = getPixelAvg();
	int nbrOfPeaks =  getNbrOfPeaks();

	if((nbrOfPeaks >= 2) && (pixel_avg < (0.4*MAX_PIX_VALUE_CALIBRATED))){ /* peak detected */
		peak_detected = 1;
		peak_timeout = 40;
		if(integrationTime_us >= 800000){ /* different timeout for longer integration time*/
			peak_timeout = 6;
		}
	}
	if(peak_detected && (peak_timeout != 0)){ /* count down peak timeout*/
			peak_timeout--;
	}
	if((pixel_avg < 0.05*MAX_PIX_VALUE_CALIBRATED) || (peak_timeout == 0)){ /* leave peakDetected Mode when average value is lower than 5% or time out is elapsed */
		peak_detected = 0;
	}
	if((pixel_avg >= 0.4*MAX_PIX_VALUE_CALIBRATED) || (nbrOfPeaks >= 2)){ /* reduce integration time */
		if(integrationTime_adaption != 1){ /* prevent integration time comes below the minimum*/
			integrationTime_us = integrationTime_us/2;
			integrationTime_adaption = integrationTime_adaption/2;
			adapt_flag = 1;
		}
	}
	if((pixel_avg < 0.2*MAX_PIX_VALUE_CALIBRATED) && (!peak_detected)){ /* increase integration time */
		integrationTime_us = integrationTime_us*2;
		integrationTime_adaption = integrationTime_adaption*2;
		adapt_flag = 1;
	}
	if(integrationTime_adaption >= 128){ /* if the integrationTime exceeds 6400ms*/
		integrationTime_us = 128*START_INTEGRATION_TIME;
		integrationTime_adaption = 128;
		adapt_flag = 0;
	}
	if(integrationTime_adaption <= 1){ /* if the integrationTime is smaller than 50ms*/
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
	for(int pix_index=0; pix_index < NUMBER_OF_PIXEL; pix_index++){
			if(sensor_data[pix_index] >= MAX_PIX_VALUE_CALIBRATED){
				peak_cntr++;
			}
	}
	return peak_cntr;
}

