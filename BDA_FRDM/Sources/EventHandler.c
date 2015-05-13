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
#include "Sensor.h"

extern State actualState;

void ProcessInitEvent(void);
void ProcessLEDHeartbeatEvent(void);
void ProcessCalibrationFinishedEvent(void);
void ProcessNewDataEvent(void);
void ProcessSensorEOS(void);
void ProcessSW1Event(void);
void ProcessSW1LongEvent(void);
void ProcessSW1ReleasedEvent(void);


EventAllocation evtAlloc[] = { { EVNT_INIT, ProcessInitEvent },
		{EVNT_LED_HEARTBEAT, ProcessLEDHeartbeatEvent },
		{EVNT_CALIBRATION_FINISHED, ProcessCalibrationFinishedEvent},{EVNT_NEW_DATA,ProcessNewDataEvent},
		{EVNT_SENSOR_EOS, ProcessSensorEOS},
		{EVNT_SW1_PRESSED, ProcessSW1Event},{EVNT_SW1_LPRESSED, ProcessSW1LongEvent},{EVNT_SW1_RELEASED, ProcessSW1ReleasedEvent}
				}; /*!< Allocation between event type and handler function*/

void EventHandler_HandleEvent(void) {
	EVNT_HandleEvent(evtAlloc, sizeof(evtAlloc));
}

void ProcessInitEvent(void) {
	//CLS1_SendStr("Hello from FRDM\r\n",CLS1_GetStdio()->stdOut);
	LED2_On();
	TRG_SetTrigger(TRG_LED2_OFF,500/TRG_TICKS_MS,LED2m_Off,NULL);
}

void TurnOffHeartBeat(TRG_CallBackDataPtr data){
	LED1_Off();
}

void ProcessLEDHeartbeatEvent(void) {
	LED1_On();
	TRG_SetTrigger(TRG_HEARTBEAT_OFF,100/TRG_TICKS_MS,TurnOffHeartBeat,NULL);
}

void ProcessCalibrationFinishedEvent(){
	COM_sendCalibrationACK();
}

void ProcessNewDataEvent(){
	COM_sendSensorData();
}

void ProcessSensorEOS(){
	if(actualState == Calibrating){
		SENSOR_handleCalibrationData();
	}
	else{
		SENSOR_handleNewData();
	}
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

