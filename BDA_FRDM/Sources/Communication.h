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

#define BUFFER_SIZE 2

struct
{ 	char command;
	char acknowledge_on;
	char repeat_cnt;
} header;

void readCommand(void);

void extractHeader(void);

void sendData(void);

void deinitUSBcom(void);


#endif /* COMMUNICATION_H_ */
