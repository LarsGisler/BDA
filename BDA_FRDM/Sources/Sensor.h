/*
 * Measure.h
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#define NUMBER_OF_PIXEL 256
#define MAX_PIX_VALUE 65535
#define CLK_TICK_US	10
// 2 interrupts per period, video_frequency is 1/4*clk_frequency
#define VIDEO_TICK_US 4*CLK_TICK_US
#define TICKS_FOR_VIDEO 2*4

#define START_INTEGRATION_TIME 1000000 //(VIDEO_TICK_US*300)

uint16_t sensor_data_raw[NUMBER_OF_PIXEL];
uint16_t sensor_data[NUMBER_OF_PIXEL];
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

void SENSOR_init(void);

byte checkIntegrationTime(void);
int getPixelAvg(void);
int getPeakAvg(void);

#endif /* MEASURE_H_ */
