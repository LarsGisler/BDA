/*
 * EventHandler.c
 *
 *  Created on: 10.03.2015
 *      Author: Cyrill
 */
#include "EventHandler.h"
#include "Led.h"
#include "WAIT1.h"
#include "Mealy.h"
#include "CLS1.h"
#include "Trigger.h"
#include "Communication.h"

void ProcessInitEvent(void);
void ProcessLEDHeartbeatEvent(void);
void ProcessCalibrationEvent(void);
void ProcessDataRequestEvent(void);
void ProcessSW1Event(void);
void ProcessSW1LongEvent(void);
void ProcessSW1ReleasedEvent(void);


EventAllocation evtAlloc[] = { { EVNT_INIT, ProcessInitEvent },
		{EVNT_LED_HEARTBEAT, ProcessLEDHeartbeatEvent },
		{EVNT_CALIBRATION, ProcessCalibrationEvent},{EVNT_DATA_REQUEST,ProcessDataRequestEvent},
		{EVNT_SW1_PRESSED, ProcessSW1Event},{EVNT_SW1_LPRESSED, ProcessSW1LongEvent},{EVNT_SW1_RELEASED, ProcessSW1ReleasedEvent}
				}; /*!< Allocation between event type and handler function*/

void EventHandler_HandleEvent(void) {
	EVNT_HandleEvent(evtAlloc, sizeof(evtAlloc));
}

void ProcessInitEvent(void) {
	CLS1_SendStr("Hello from FRDM\r\n",CLS1_GetStdio()->stdOut);
	for (int i = 0; i < 3; i++) {
		LED2_On();
		WAIT1_Waitms(100);
		LED2_Off();
		WAIT1_Waitms(100);
	}
}

void TurnOffHeartBeat(TRG_CallBackDataPtr data){
	LED1_Off();
}

void ProcessLEDHeartbeatEvent(void) {
	LED1_On();
	TRG_SetTrigger(TRG_HEARTBEAT_OFF,100/TRG_TICKS_MS,TurnOffHeartBeat,NULL);
}

void ProcessCalibrationEvent(){
	COM_sendCalibrationACK();
}

void ProcessDataRequestEvent(){
	COM_sendSensorData();
}

void ProcessSW1Event(void) {
	//CLS1_SendStr("A Pressed\r\n", CLS1_GetStdio()->stdOut);
	Led2_NegVal();
}
void ProcessSW1LongEvent(void){
	//CLS1_SendStr("A Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW1ReleasedEvent(void){
	//CLS1_SendStr("A Released\r\n", CLS1_GetStdio()->stdOut);
}

