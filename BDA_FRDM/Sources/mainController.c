/**
 * \file
 * \brief initializations and starts the program
 * \author Lars Gisler
 *
 * This modul starts up the program, initializes all other moduls, creates the mixed task and starts the FreeRTOS
 */

#include "mainController.h"
#include "WAIT1.h"
#include "Led.h"
#include "Event.h"
#include "EventHandler.h"
#include "Timer.h"
#include "Cpu.h"
#include "Platform.h"
#include "CLS1.h"
#include "FRTOS1.h"
#include "RTOS.h"
#include "Trigger.h"
#include "AD1.h"
#include "ST.h"
#include "EN.h"
#include "LEDred.h"
#include "Sensor.h"
#include "Communication.h"


/*
 * Task to handle events and receive commands over USB
 */
static portTASK_FUNCTION(MixedTask, pvParameters) {

	for (;;) {
		EventHandler_HandleEvent();

		while (CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer)) == ERR_BUSOFF) {
		}
		COM_readCommand();
		COM_extractCommandInfo();

		FRTOS1_vTaskDelay(10 / portTICK_RATE_MS);
	}
}



void mainController_run(void) {
	PL_Init();
	EVNT_SetEvent(EVNT_INIT);

	if (FRTOS1_xTaskCreate(MixedTask, (signed portCHAR *)"Mixed Task", configMINIMAL_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
		for(;;){} /* error */
	}
	RTOS_Run();
}

