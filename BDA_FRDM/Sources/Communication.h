/**
 * \file
 * \brief Communication Driver
 * \author Lars Gisler
 *
 * This module implements the USB communication. It builts up on the processor expert USB CDC component.
 * It handles the communication protocol and provides methods to receive and send commands and data over USB.
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "Platform.h"
#if PL_HAS_RTOS
#include "FRTOS1.h"
#endif
#include "USB1.h"

/*! Bitmasks to handle protocol header*/
#define COMMAND_MASK 0b00001111
#define SERVICE_MASK 0b00001000
#define REPEAT_MASK 0b11110000

/*! Protocol: receiving Commands*/
#define CALIBRATION_COMMAND 0b0011
#define SEND_DATA_COMMAND 0b0110

/*! Protocol: sending Commands*/
#define CALIBRATION_ACK_COMMAND 0b0100
#define MEASURED_DATA_END 0b1111111111111111

#define BUFFER_SIZE 2

/*! buffer for USB CDC*/
extern uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];

/*! semaphores are used for inter-process communication*/
extern xSemaphoreHandle sem_calibration; /* indicates that the calibration process is finished */
extern xSemaphoreHandle sem_dataCommand; /* indicates that new data was requested over USB*/

/*! a struct to store information received over USB*/
struct
{ 	char command;	/*!< command*/
	char acknowledge_on;	/*!< shows whether the service protocol is on and acknowledge is requested*/
	char repeat_cnt;	/*!< shows the number of attempts needed to send the message*/
} header;

/*!
 * \brief gets the message from the USB buffer and saves it in the protocol header
 */
void COM_readCommand(void);

/*!
 * \brief extracts the received command and sets the program in the correct state or releases the correct semaphore
 */
void COM_extractCommandInfo(void);

/*!
 * \brief sends the acknowledge after the calibration is finished
 */
void COM_sendCalibrationACK(void);

/*!
 * \brief sends a single pixel value
 *  empties the entire USB buffer by calling CDC_App_Task function
 */
void COM_sendPixel(uint8_t pix_index);

/*!
 * \brief sends the sensor data by going trough the sensor data with a for loop
 *  the end of the sending process is confirmed by a DataEnd-Command
 */
void COM_sendSensorData(void);

/*!
 * \brief sends the DataEnd-Command
 * empties the entire USB buffer by calling CDC_App_Task function
 */
void COM_sendMeasuredDataEnd(void);

/*!
 * \brief builds up the protocol header according to the configured protocol type
 * \param[in] command which needs to be embedded into the header
 * \return complete header
 */
uint16_t buildProtocolHeader(uint8_t command);

/*!
 * \brief initializes the USB communication
 */
void COM_Init(void);

/*!
 * \brief deinitializes the USB communication
 * creates semaphore to indicate end of calibration
 * creates semapohre to indicate received data comand
 */
void COM_Deinit(void);


#endif /* COMMUNICATION_H_ */
