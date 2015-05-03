/*
 * Communication.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#include "Platform.h"
#include "Cpu.h"
#include "Measure.h"
#include "Communication.h"
#include "USB1.h"


extern uint16_t measured_data[];

uint8_t raw_header[BUFFER_SIZE];

void readCommand(){
	CDC1_GetChar(&raw_header[0]);
	CDC1_GetChar(&raw_header[1]);
}

void extractHeader(){
	header.command = (raw_header[0] & COMMAND_MASK);
	header.acknowledge_on = (raw_header[1] & SERVICE_MASK)>>3;
	header.repeat_cnt = (raw_header[1] & REPEAT_MASK)>>4;
}

void sendData(){
	//for(int a=0;a<sizeof(measured_data);a++){
		CDC1_SendChar((char) measured_data[0]);
		CDC1_SendChar((char) (measured_data[0]>>8));
	//}
}

void deinitUSBcom(){
	header.command = 0;
	raw_header[0] = 0;
	raw_header[1] = 0;
}

