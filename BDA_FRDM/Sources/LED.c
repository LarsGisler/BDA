/*
 * LED.c
 *
 *  Created on: 02.03.2015
 *      Author: tastyger
 */

#include "LED.h"

void LED_Init(void) {
  /* all LED's off by default */
  LED1_Off();
  LED2_Off();
  LED3_Off();
}

void LED_Deinit(void) {
  /* nothing to do */
}

void LED2m_Off(){
	LED2_Off();
}

void LED3m_Off(){
	LED3_Off();
}
