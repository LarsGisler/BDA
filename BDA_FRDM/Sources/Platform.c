/*
 * Platform.c
 *
 *  Created on: 02.03.2015
 *      Author: tastyger
 */

#include "Platform.h"
#include "LED.h"
#include "Mealy.h"
#include "Keys.h"
#include "Event.h"
#include "Trigger.h"
#include "Buzzer.h"
#include "RTOS.h"
#include "Shell.h"
#include "ShellQueue.h"

void PL_Init(void) {
#if PL_HAS_MEALY
	MEALY_Init();
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

#if PL_HAS_TRIGGER
	TRG_Init();
#endif
#if PL_HAS_BUZZER
	BUZ_Init();
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

#if PL_HAS_MEALY
	MEALY_Deinit();
#endif
#if PL_HAS_LED
	LED_Deinit();
#endif

#if PL_HAS_EVENTS
	EVNT_Deinit();
#endif

#if PL_HAS_KEYS
	KEY_Deinit();
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
