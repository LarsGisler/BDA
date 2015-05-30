/**
 * \file
 * \brief provides functions which after certain events occurred
 * \author Cyrill Wyss and Lars Gisler
 *
 */
#include "EventHandler.h"
#include "Led.h"
#include "WAIT1.h"
#include "CLS1.h"
#include "Trigger.h"
#include "Communication.h"
#include "Sensor.h"

extern State actualState;

void ProcessInitEvent(void);
//void ProcessLEDHeartbeatEvent(void);
void ProcessSW1Event(void);


EventAllocation evtAlloc[] = { { EVNT_INIT, ProcessInitEvent },
		//{EVNT_LED_HEARTBEAT, ProcessLEDHeartbeatEvent },
		{EVNT_SW1_PRESSED, ProcessSW1Event}
				}; /*!< Allocation between event type and handler function*/

void EventHandler_HandleEvent(void) {
	EVNT_HandleEvent(evtAlloc, sizeof(evtAlloc));
}

/*!
 * \brief init function to indicate system start with a LED
 */
void ProcessInitEvent(void) {
	LED2_On();
	TRG_SetTrigger(TRG_LED2_OFF,500/TRG_TICKS_MS,LED2m_Off,NULL);
}

/*!
 * \brief
 */
void TurnOffHeartBeat(TRG_CallBackDataPtr data){
	LED1_Off();
}

/*!
 * \brief creates an heartbeat with the on board LED of the FRDM-Board
 */
void ProcessLEDHeartbeatEvent(void) {
	LED1_On();
	TRG_SetTrigger(TRG_HEARTBEAT_OFF,100/TRG_TICKS_MS,TurnOffHeartBeat,NULL);
}

/*!
 * \brief optional function which is executed after SW1 is pressed
 */
void ProcessSW1Event(void) {
	Led2_NegVal();
}
void ProcessSW1LongEvent(void){
}
void ProcessSW1ReleasedEvent(void){
}

