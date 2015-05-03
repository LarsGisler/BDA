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
#include "Trigger.h"
#include "Led.h"



extern uint16_t sensor_data_ready[];

uint8_t header_buffer[BUFFER_SIZE];


void COM_readCommand(){
	CDC1_GetChar(&header_buffer[0]);
	CDC1_GetChar(&header_buffer[1]);

	header.command = (header_buffer[0] & COMMAND_MASK);
	header.acknowledge_on = (header_buffer[1] & SERVICE_MASK)>>3;
	header.repeat_cnt = (header_buffer[1] & REPEAT_MASK)>>4;

}



void COM_extractCommandInfo(){
	if (header.command != 0) {
		LED3_On(); TRG_SetTrigger(TRG_LED3_OFF, 100, LED3m_Off, NULL);
		switch(header.command){
			case SEND_DATA_COMMAND: COM_sendPixel(0);
				break;
			case CALIBRATE_COMMAND:
				break;
			default: break;
		}
		header.command = 0;
		header_buffer[0] = 0;
		header_buffer[1] = 0;
	}

}


void COM_sendPixel(uint8_t pix_index){
	uint8_t byteL = sensor_data_ready[pix_index];
	uint8_t byteH = sensor_data_ready[pix_index]>>8;
	CDC1_SendChar((char)byteL);
	CDC1_SendChar((char)byteH);
}




