/*
 * Communication.h
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "Platform.h"
#if PL_HAS_RTOS
#include "FRTOS1.h"
#endif

// Bitmasks to handle protocol header
#define COMMAND_MASK 0b00001111
#define SERVICE_MASK 0b00001000
#define REPEAT_MASK 0b11110000

// Protocol receiving Commands
#define CALIBRATION_COMMAND 0b0011
#define SEND_DATA_COMMAND 0b0110

// Protocol sending Commands
#define CALIBRATION_ACK_COMMAND 0b0100
#define MEASURED_DATA_END 0b1111111111111111

#define BUFFER_SIZE 2

extern xSemaphoreHandle sem_calibration;
extern xSemaphoreHandle sem_dataCommand;

struct
{ 	char command;
	char acknowledge_on;
	char repeat_cnt;
} header;

void COM_readCommand(void);

void COM_extractCommandInfo(void);

void COM_sendCalibrationACK(void);

void COM_sendPixel(uint8_t pix_index);

void COM_sendSensorData(void);

void COM_sendMeasuredDataEnd(void);

uint16_t buildProtocolHeader(uint8_t command);

void COM_Init(void);
void COM_Deinit(void);


#endif /* COMMUNICATION_H_ */
