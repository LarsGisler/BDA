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
#include "SHDN.h"
#include "LEDred.h"
#include "EN.h"
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
	for (int p_index = 0; p_index < NUMBER_OF_PIXEL; p_index++) {
		sensor_data[p_index] = p_index * 100;
	}
}

void SENSOR_CLK_interrupt() {
	if (actualState != Starting) {
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

			if (clk_cntr == TICKS_FOR_VIDEO) {
				measurePixel(pix_index);
				clk_cntr = 1;
				pix_index++;
			}
			else {
				clk_cntr++;
			}
			if (clk_cntr == 4) {
				ST_SetVal();
			}
		}
	}
}

/*uint8_t SENSOR_measureIntegrationTime() {
 static int integrationTime_cntr = 0;
 if (integrationTime_cntr == (integrationTime_us / CLK_TICK_US)) {
 integrationTime_cntr++;
 return 1;
 } else {
 return 0;
 }
 }*/

void SENSOR_Start() {
	//(void)TU2_Enable(NULL);
	actualState = Waiting;
}

void SENSOR_handleCalibrationData() {
	for (int p_index = 0; p_index < NUMBER_OF_PIXEL; p_index++) {
		sensor_calibration_data[p_index] = sensor_data_raw[p_index];
		sensor_data_raw[p_index] = 0;
	}
	//integrationTime_us = START_INTEGRATION_TIME;
	EVNT_SetEvent(EVNT_CALIBRATION_FINISHED);
}

void measurePixel() {
	TestPin_SetVal();
	(void) AD1_Measure(FALSE);



}

/*void SENSOR_readSensor() {
	static int8_t clk_cntr = -6;

	if (clk_cntr == TICKS_FOR_VIDEO) {
		measurePixel(pix_index);
		clk_cntr = 1;
		if (pix_index == (NUMBER_OF_PIXEL - 1)) {
			pix_index = 0;
			clk_cntr = -6;
		} else {
			pix_index++;
		}
	} else {
		clk_cntr++;
	}
	if (clk_cntr == -3) {
		ST_SetVal();
	}
}*/

/*void SENSOR_startMeasurement() {
 ST_ClrVal();
 readingData_flag = 1;
 //integrationTime_us = START_INTEGRATION_TIME;
 }*/

void SENSOR_EOS_interrupt() {
	readingData_flag = 0;
	EVNT_SetEvent(EVNT_SENSOR_EOS);
}

void SENSOR_handleNewData() {
	static int a_cntr = 0;
	CS1_CriticalVariable()
	;
	CS1_EnterCritical()
	;
	for (int p_index = 0; p_index < NUMBER_OF_PIXEL; p_index++) {
		sensor_data[p_index] = sensor_data_raw[p_index];
		sensor_data_raw[p_index] = 0;
	}
	CS1_ExitCritical()
	;
	/*if (checkIntegrationTime()) {
	 actualState = Measuring;
	 } else {*/
	EVNT_SetEvent(EVNT_NEW_DATA);
}

void SENSOR_init() {
	readingData_flag = 0;
	integrationTime_adaption = 1;
	pix_index = 0;
	LEDred_SetVal();
	EN_SetVal();
	SHDN_SetVal();
#if !PL_HAS_SENSOR
	SENSOR_loadDummyData();
#endif
	actualState = Starting;
}

byte checkIntegrationTime() {
	int pixel_avg = getPixelAvg();
	int peak_avg = getPeakAvg();
	// write code here...
	if (pixel_avg < MAX_PIX_VALUE) {
		integrationTime_us = integrationTime_us * 2;
		integrationTime_adaption = integrationTime_adaption * 2;
	}

}
int getPixelAvg() {
	int sum = 0;
	for (uint8_t pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		sum = sum + sensor_data[pix_index];
	}
	return sum / 256;
}

int getPeakAvg() {
	byte peak_cntr = 0;
	int peak_sum = 0;
	int pix_avg = getPixelAvg();
	for (uint8_t pix_index = 0; pix_index < NUMBER_OF_PIXEL; pix_index++) {
		if (sensor_data[pix_index] > (3 * pix_avg)) {
			peak_cntr++;
			peak_sum = peak_sum + sensor_data[pix_index];
		}
	}
	if ((peak_cntr > 2) && (peak_cntr < 40)) {
		return peak_sum / peak_cntr; // there is a peak in the spectrum, return average value of peak pixels
	} else {
		return 0; // there is no peak in the spectrum, even distribution of pixels
	}
}

