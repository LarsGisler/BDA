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

void ProcessInitEvent(void);
void ProcessSW1Event(void);
void ProcessSW2Event(void);
void ProcessSW3Event(void);
void ProcessSW4Event(void);
void ProcessSW5Event(void);
void ProcessSW6Event(void);
void ProcessSW7Event(void);
void ProcessSW1EventLong(void);
void ProcessSW2EventLong(void);
void ProcessSW3EventLong(void);
void ProcessSW4EventLong(void);
void ProcessSW5EventLong(void);
void ProcessSW6EventLong(void);
void ProcessSW7EventLong(void);
void ProcessSW1EventReleased(void);
void ProcessSW2EventReleased(void);
void ProcessSW3EventReleased(void);
void ProcessSW4EventReleased(void);
void ProcessSW5EventReleased(void);
void ProcessSW6EventReleased(void);
void ProcessSW7EventReleased(void);
void ProcessLEDHeartbeatEvent(void);

EventAllocation evtAlloc[] = { { EVNT_INIT, ProcessInitEvent },
		{EVNT_LED_HEARTBEAT, ProcessLEDHeartbeatEvent },
		{EVNT_SW1_PRESSED, ProcessSW1Event },
		{ EVNT_SW2_PRESSED, ProcessSW2Event }, { EVNT_SW3_PRESSED,
				ProcessSW3Event }, { EVNT_SW4_PRESSED, ProcessSW4Event }, {
				EVNT_SW5_PRESSED, ProcessSW5Event }, { EVNT_SW6_PRESSED,
				ProcessSW6Event }, { EVNT_SW7_PRESSED, ProcessSW7Event },

				{ EVNT_SW1_LPRESSED, ProcessSW1EventLong },{ EVNT_SW2_LPRESSED, ProcessSW2EventLong },{ EVNT_SW3_LPRESSED, ProcessSW3EventLong },{ EVNT_SW4_LPRESSED, ProcessSW4EventLong },{ EVNT_SW5_LPRESSED, ProcessSW5EventLong },{ EVNT_SW6_LPRESSED, ProcessSW6EventLong },{ EVNT_SW7_LPRESSED, ProcessSW7EventLong },
				{ EVNT_SW1_RELEASED, ProcessSW1EventReleased },{ EVNT_SW2_RELEASED, ProcessSW2EventReleased },{ EVNT_SW3_RELEASED, ProcessSW3EventReleased },{ EVNT_SW4_RELEASED, ProcessSW4EventReleased },{ EVNT_SW5_RELEASED, ProcessSW5EventReleased },{ EVNT_SW6_RELEASED, ProcessSW6EventReleased },{ EVNT_SW7_RELEASED, ProcessSW7EventReleased }
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



void ProcessSW1Event(void) {

	CLS1_SendStr("A Pressed\r\n", CLS1_GetStdio()->stdOut);
	LED2_On();

}
void ProcessSW2Event(void) {
	CLS1_SendStr("B Pressed\r\n", CLS1_GetStdio()->stdOut);
	LED2_Off();
}
void ProcessSW3Event(void) {
	CLS1_SendStr("C Pressed\r\n", CLS1_GetStdio()->stdOut);

}
void ProcessSW4Event(void) {
	CLS1_SendStr("D Pressed\r\n", CLS1_GetStdio()->stdOut);

}
void ProcessSW5Event(void) {
	CLS1_SendStr("E Pressed\r\n", CLS1_GetStdio()->stdOut);

}
void ProcessSW6Event(void) {
	CLS1_SendStr("F Pressed\r\n", CLS1_GetStdio()->stdOut);

}
void ProcessSW7Event(void) {
	CLS1_SendStr("Joystick Pressed\r\n", CLS1_GetStdio()->stdOut);

}
void ProcessSW1EventLong(void)
{
	CLS1_SendStr("A Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW2EventLong(void)
{
	CLS1_SendStr("B Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW3EventLong(void)
{
	CLS1_SendStr("C Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW4EventLong(void){
	CLS1_SendStr("D Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW5EventLong(void){
	CLS1_SendStr("E Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW6EventLong(void){
	CLS1_SendStr("F Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW7EventLong(void){
	CLS1_SendStr("Joystick Long Pressed\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW1EventReleased(void){
	CLS1_SendStr("A Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW2EventReleased(void){
	CLS1_SendStr("B Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW3EventReleased(void){
	CLS1_SendStr("C Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW4EventReleased(void){
	CLS1_SendStr("D Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW5EventReleased(void){
	CLS1_SendStr("E Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW6EventReleased(void){
	CLS1_SendStr("F Released\r\n", CLS1_GetStdio()->stdOut);
}
void ProcessSW7EventReleased(void){
	CLS1_SendStr("Joystick Released\r\n", CLS1_GetStdio()->stdOut);
}

