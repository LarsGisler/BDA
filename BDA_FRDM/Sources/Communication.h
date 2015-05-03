/*
 * Communication.h
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define COMMAND_MASK 0b00001111
#define SERVICE_MASK 0b00001000
#define REPEAT_MASK 0b11110000

#define SEND_DATA_COMMAND 0b0011
#define CALIBRATE_COMMAND 0b0110

#define BUFFER_SIZE 2

struct
{ 	char command;
	char acknowledge_on;
	char repeat_cnt;
} header;

void COM_readCommand(void);

void COM_extractCommandInfo(void);

void COM_sendPixel(uint8_t pix_index);

void COM_deinitUSBcom(void);


#endif /* COMMUNICATION_H_ */
