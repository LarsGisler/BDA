/*
 * mainController.c
 *
 *  Created on: 24.02.2015
 *      Author: Cyrill
 */

#include "mainController.h"
#include "WAIT1.h"
#include "Led.h"
#include "Event.h"
#include "EventHandler.h"
#include "Timer.h"
#include "Cpu.h"
#include "Mealy.h"
#include "Platform.h"
#include "Keys.h"
#include "CLS1.h"
//#include "FRTOS1.h"
#include "RTOS.h"
#include "Shell.h"
#include "USB1.h"
#include "Trigger.h"

static uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];
static uint8_t received_char = 0;
uint8_t measured_value = 55;

static portTASK_FUNCTION(Main, pvParameters) {
	KEY_EnableInterrupts();
	EVNT_SetEvent(EVNT_INIT);
	for (;;) {

		}

		/*
		 KEY_Scan();
		 EventHandler_HandleEvent();

		 CDC1_GetChar(&received_data);
		 if(received_data != 0){
		 CDC1_SendChar(received_data);
		 received_data = 0;
		 }

		 FRTOS1_vTaskDelay(50 / portTICK_RATE_MS);
		 */

}

void mainController_run(void) {

	PL_Init();

	KEY_EnableInterrupts();
	EVNT_SetEvent(EVNT_INIT);
	int cnt = 0;
	int valueToSend = 48;

	for (;;) {
		KEY_Scan();
		EventHandler_HandleEvent();

		while (CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer)) == ERR_BUSOFF) {
			WAIT1_Waitms(10);
		}
		CDC1_GetChar(&received_char);
		if (received_char != 0) {
			//CLS1_SendChar(received_char);
			LED3_On();
			TRG_SetTrigger(TRG_LED3_OFF, 100, LED3m_Off, NULL);
			(void)CDC1_SendChar(received_char);
			received_char = 0;
		} else {
			WAIT1_Waitms(10);
			cnt++;
			if ((cnt % 300) == 0) {  //send the number from time to time
				(void)CDC1_SendChar((char)valueToSend);
				valueToSend++;
				if(valueToSend == 57){
					valueToSend = 48;

				}
			}
		}
	}

	//if (FRTOS1_xTaskCreate(Main, (signed portCHAR *)"MAIN", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS) {
	//   for(;;){} /* error */
	//}
	//RTOS_Run();
}


