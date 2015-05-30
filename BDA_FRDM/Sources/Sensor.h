/**
 * \file
 * \brief Sensor Driver
 * \author Lars Gisler
 *
 * This module provides all functions to communicate with the Hamamatsu Sensor and to read and store the sensor data.
 * It ensures the synchronization between the A/D conversion and the sensor video signal. The modul furthermore provides
 * an algorithm to dynamically adapt the integration time.
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#include "Platform.h"
#if PL_HAS_RTOS
#include "FRTOS1.h"
#endif

#define NUMBER_OF_PIXEL 256
#define MAX_PIX_VALUE 65535
#define MAX_PIX_VALUE_CALIBRATED 58235 /* assume a sensor offset of 7300 */
#define CLK_TICK_US	20
#define TICKS_FOR_VIDEO (2*4) /* 2 interrupts per period, video_frequency is 1/4*clk_frequency */

#define START_INTEGRATION_TIME 50000 /* minimal integration time: VIDEO_TICK_US*256 + reserve time */

/*! semaphores are used for inter-process communication*/
extern xSemaphoreHandle sem_dataAvailable; /* indicates that new data from the sensor is available*/
extern xSemaphoreHandle sem_EOS;	/* indicates that an EOS interrupt occured*/

extern int integrationTime_us;
/*! unmodified sensor data*/
uint16_t sensor_data_raw[NUMBER_OF_PIXEL];
/*! sensor data with offset removed*/
uint16_t sensor_data[NUMBER_OF_PIXEL];
/*! sensor data ready to send*/
extern uint16_t sensor_data_ready[NUMBER_OF_PIXEL];
/*! calibration data*/
uint16_t sensor_calibration_data[NUMBER_OF_PIXEL];
/* global index to iterate through the sensor data*/
extern uint8_t pixel_index;

/*! program states*/
typedef enum State {
  Starting, /*!< starting state */
  Calibrating, /*!< calibrating state */
  Measuring, /*!< measuring state */
} State;

extern State actualState;

/*!
 * \brief loads dummy data for simulation or demonstration purposes
 * the dummy data set starts with 0 and increases by 100 for every value
 */
void SENSOR_loadDummyData(void);

/*!
 * \brief synchronizes the ADC with the sensor output signal by means of a counter variable
 * this method is executed by the timer interrupt (on every edge of the clk signal)
 * the counter variable and the integration time determine which part of the method is executed
 */
void SENSOR_CLK_interrupt(void);

/*!
 * \brief handles the complete calibration process
 * the method is called three times till the whole process is completed
 */
void SENSOR_handleCalibrationData();

/*!
 * \brief handles new data which was successfully read by the senosr
 * the method calculates the correct value for the tablet application considering the calibration data
 * and the integration time.
 */
void SENSOR_handleNewData(void);

/*!
 * \brief starts the ADC. (ADC is stopped by an interrupt when completed)
 */
void measurePixel();

/*!
 * \brief initialization of the sensor module. creates semaphores and tasks.
 */
void SENSOR_Init(void);

/*!
 * \brief deinitialization of the sensor module
 */
void SENSOR_Deinit(void);

/*!
 * \brief makes average and peak based calculation to adapt the integration time
 * \return true is the integration time is changed. FALSE when it stays the same
 */
byte adaptIntegrationTime(void);

/*!
 * \brief calculates the average of all measured pixel values
 * \return the average of the pixel values
 */
int getPixelAvg(void);

/*!
 * \brief calculates the number of peaks in one complete measure cycle a peak means a saturated pixel
 * \return the number of peaks
 */
int getNbrOfPeaks(void);

#endif /* MEASURE_H_ */
