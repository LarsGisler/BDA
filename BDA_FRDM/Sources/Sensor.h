/*
 * Sensor.h
 *
 *  Created on: 03.05.2015
 *      Author: Lars Gisler
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#include "Platform.h"
#if PL_HAS_RTOS
#include "FRTOS1.h"
#endif

#define NUMBER_OF_PIXEL 256
#define MAX_PIX_VALUE 65535
// assume a sensor offset of 7300
#define MAX_PIX_VALUE_CALIBRATED 58235
#define CLK_TICK_US	20
// 2 interrupts per period, video_frequency is 1/4*clk_frequency
#define TICKS_FOR_VIDEO (2*4)

#define START_INTEGRATION_TIME 50000 //(VIDEO_TICK_US*300)

extern xSemaphoreHandle sem_dataAvailable;
extern xSemaphoreHandle sem_EOS;

uint16_t sensor_data_raw[NUMBER_OF_PIXEL];
uint16_t sensor_data[NUMBER_OF_PIXEL];
uint16_t sensor_data_ready[NUMBER_OF_PIXEL];
uint16_t sensor_calibration_data[NUMBER_OF_PIXEL];
uint8_t pix_index;
int8_t clk_cntr;

typedef enum State {
  Waiting,
  Starting,
  Calibrating,
  Measuring,
} State;

void SENSOR_loadDummyData(void);

void SENSOR_CLK_interrupt(void);

void SENSOR_handleCalibrationData();

void SENSOR_Start(void);

void measurePixel();

void SENSOR_readSensor(void);

void SENSOR_startMeasurement(void);

uint8_t SENSOR_measureIntegrationTime(void);

void SENSOR_EOS_interrupt(void);

void SENSOR_handleNewData(void);

void SENSOR_Init(void);
void SENSOR_Deinit(void);

byte adaptIntegrationTime(void);
int getPixelAvg(void);
int getNbrOfPeaks(void);

#endif /* MEASURE_H_ */
