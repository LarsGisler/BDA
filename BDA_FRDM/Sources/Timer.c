/*
 * Timer.c
 *
 *  Created on: 10.03.2015
 *      Author: Cyrill
 */
#include "Timer.h"

#if PL_HAS_HAS_TIMER
#include "Event.h"
#include "Trigger.h"

#define TICKS_FOR_HEARTBEAT (1000000/TMR_TICK_US)


void TMR_OnInterrupt(void){
	  static int cntr=0;

	#if PL_HAS_TRIGGER
	  TRG_IncTick();
	#endif
	  cntr++;
	  if (cntr==(10000000/TMR_TICK_US)) {
	#if 1 /* setting an event */
	    EVNT_SetEvent(EVNT_LED_HEARTBEAT);
	#else /* toggling directly the LED */
	    LED1_Neg();
	#endif
	    cntr = 0;
	  }
}


void TMR_Init(void){

}


void TMR_Deinit(void){

}


#endif
