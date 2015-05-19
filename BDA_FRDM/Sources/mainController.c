/*
 * mainController.c
 *
 *  Created on: 24.02.2015
 *      Author: Lars Gisler
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
#include "Shell.h"
#include "USB1.h"
#include "Trigger.h"
#include "AD1.h"
#include "ST.h"
#include "EN.h"
#include "LEDred.h"
#include "Sensor.h"
#include "Communication.h"

extern State actualState;

extern uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];

static portTASK_FUNCTION(Main, pvParameters) {

	for (;;) {
		EventHandler_HandleEvent();
		while (CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer)) == ERR_BUSOFF) {
					//WAIT1_Waitms(10);
		}
		COM_readCommand();
		COM_extractCommandInfo();
		FRTOS1_vTaskDelay(20 / portTICK_RATE_MS);
	}
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


void mainController_run(void) {
	PL_Init();

	EVNT_SetEvent(EVNT_INIT);
	if (FRTOS1_xTaskCreate(Main, (signed portCHAR *)"MAIN", configMINIMAL_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
		for(;;){} /* error */
	}
	RTOS_Run();
}

		/*switch(actualState){
		case Waiting:
			COM_extractCommandInfo();
			break;
		case Starting:
			//SENSOR_Start();
			COM_extractCommandInfo();
			break;
		case Calibrating:
			COM_extractCommandInfo();
			break;
		case Measuring:
			COM_extractCommandInfo();
			break;
		default: break;
		}
	}*/

		/*else {
				WAIT1_Waitms(10);
				cnt++;
				if ((cnt % 300) == 0) {  //send the number from time to time
					(void) CDC1_SendChar((char) valueToSend);
					valueToSend++;
					if (valueToSend == 57) {
						valueToSend = 48;

					}
				}
			}

}*/

//if (FRTOS1_xTaskCreate(Main, (signed portCHAR *)"MAIN", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS) {
//   for(;;){} /* error */
//}
//RTOS_Run();


