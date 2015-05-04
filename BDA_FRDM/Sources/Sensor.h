/*
 * Measure.h
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#define NUMBER_OF_PIXEL 256
#define CLK_TICK_US	10
// 2 interrupts per period, video_frequency is 1/4*clk_frequency
#define VIDEO_TICK_US 80
#define TICKS_FOR_VIDEO (2*4)

#define START_INTEGRATION_TIME (VIDEO_TICK_US*300)

void SENSOR_loadDummyData(void);

void SENSOR_CLK_interrupt(void);

void measurePixel(uint8_t pix_index);

void SENSOR_readSensor(void);

void SENSOR_startMeasurement(void);

uint8_t SENSOR_measureIntegrationTime(void);

void SENSOR_EOS_interrupt(void);

void SENSOR_handleNewData(void);




#endif /* MEASURE_H_ */
