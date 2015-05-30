/*
 * Platform.c
 *
 *  Created on: 02.03.2015
 *      Author: tastyger
 */

#include "Platform.h"
#include "Led.h"
#include "Event.h"
#include "Trigger.h"
#include "RTOS.h"
#include "Sensor.h"
#include "Communication.h"

void PL_Init(void) {

#if PL_HAS_SENSOR
	SENSOR_Init();
#endif
#if PL_HAS_COM
	COM_Init();
#endif

#if PL_HAS_LED
	LED_Init();
#endif

#if PL_HAS_EVENTS
	EVNT_Init();
#endif

#if PL_HAS_RTOS
	 RTOS_Init();
#endif

#if PL_HAS_TIMER
	 TMR_Init();
#endif

#if PL_HAS_TRIGGER
	 TRG_Init();
#endif
}

void PL_Deinit(void) {

#if PL_HAS_SENSOR
	SENSOR_Deinit();
#endif

#if PL_HAS_COM
	COM_Deinit();
#endif

#if PL_HAS_LED
	LED_Deinit();
#endif

#if PL_HAS_EVENTS
	EVNT_Deinit();
#endif

#if PL_HAS_TIMER
	TMR_Deinit();
#endif

#if PL_HAS_TRIGGER
	TRG_Deinit();
#endif

#if PL_HAS_RTOS
	RTOS_Deinit();
#endif

}
