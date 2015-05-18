/*
 * Platform.c
 *
 *  Created on: 02.03.2015
 *      Author: tastyger
 */

#include "Platform.h"
#include "LED.h"
#include "Event.h"
#include "Trigger.h"
#include "RTOS.h"
#include "Shell.h"
#include "ShellQueue.h"
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

#if PL_HAS_KEYS
	KEY_Init();
#endif

#if PL_HAS_RTOS
	 RTOS_Init();
#endif
#if PL_HAS_SHELL_QUEUE
	 SQUEUE_Init();
#endif
#if	PL_HAS_SHELL
	 SHELL_Init();
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

#if PL_HAS_TRIGGER
	TRG_Deinit();
#endif

#if PL_HAS_BUZZER
	BUZ_Deinit();
#endif

#if PL_HAS_RTOS
	 RTOS_Deinit();
#endif
#if	PL_HAS_SHELL
	 SHELL_Deinit();
#endif
#if PL_HAS_SHELL_QUEUE
	 SQUEUE_Deinit();
#endif
}
