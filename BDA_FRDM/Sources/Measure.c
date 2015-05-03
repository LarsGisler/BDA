/*
 * Measure.c
 *
 *  Created on: 03.05.2015
 *      Author: Lars
 */

#include "Platform.h"
#include "Cpu.h"
#include "Measure.h"
#include "AD1.h"


uint16_t sensor_data[NUMBER_OF_PIXEL];
uint16_t sensor_data_ready[NUMBER_OF_PIXEL];

void loadDummyData(){
	for(int a = 0; a<NUMBER_OF_PIXEL; a++){
		sensor_data_ready[a] = a*100;
	}
}

void measurePixel(uint8_t pix_index){
	(void) AD1_Measure(TRUE);
	(void) AD1_GetValue16(&sensor_data[pix_index]);
}
