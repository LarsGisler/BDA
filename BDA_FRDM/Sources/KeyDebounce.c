/**
 * \file
 * \brief Key debouncing implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements debouncing of up to 4 Keys.
 */

#include "Platform.h"
#if PL_HAS_DEBOUNCE

#include "Keys.h"
#include "Debounce.h"
#include "Trigger.h"
#include "Event.h"
#include "CS1.h"

/*!
 * \brief Returns the state of the keys. This directly reflects the value of the port
 * \return Port bits
 */
static DBNC_KeySet KEYDBNC_GetKeys(void) {
	DBNC_KeySet keys = 0;

#if PL_NOF_KEYS >= 1
	if (KEY1_Get()) {
		keys |= (1 << 0);
	}
#endif
	return keys;
}

/*!
 * \brief Event called in the event of a long key press detected.
 * \param keys The keyboard port content
 * \param event The event kind
 */
static void KEYDBNC_OnDebounceEvent(DBNC_EventKinds event, DBNC_KeySet keys) {
	switch (event) {
	/* pressed */
	case DBNC_EVENT_PRESSED:
#if PL_NOF_KEYS >= 1
		if (keys == (1 << 0)) {
			EVNT_SetEvent(EVNT_SW1_PRESSED);
		}
#endif
		break;

		/* long pressed */
	case DBNC_EVENT_LONG_PRESSED:
#if PL_NOF_KEYS >= 1
		if (keys == (1 << 0)) {
			EVNT_SetEvent(EVNT_SW1_LPRESSED);
		}
#endif
		break;

		/* released */
	case DBNC_EVENT_RELEASED:
#if PL_NOF_KEYS >= 1
		if (keys == (1 << 0)) {
			EVNT_SetEvent(EVNT_SW1_RELEASED);
		}
#endif

		break;
	case DBNC_EVENT_END:
		KEY_EnableInterrupts();
		/*! \todo Implement what you have to do at the end of the debouncing. Need to re-enable interrupts? */
		break;
	} /* switch */
}

/*! \brief This struct contains all the data and pointers needed to have
 *         our debounce FSM reentrant and usable for multiple ISR's.
 */
static DBNC_FSMData KEYDBNC_FSMdata = {
/* callbacks: */
KEYDBNC_GetKeys, /* returns bit set of pressed keys */
KEYDBNC_OnDebounceEvent, /* event callback */
/* data: */
DBNC_KEY_IDLE, /* initial state machine state, here the state is stored */
0, /* key scan value */
0, /* long key count */
TRG_KEYPRESS, /* trigger to be used */
(50 / TRG_TICKS_MS), /* debounceTicks for 50 ms */
(500 / TRG_TICKS_MS), /* longKeyTicks for 500 ms */
};

void KEYDBNC_Scan(void) {

	if (KEYDBNC_FSMdata.state == DBNC_KEY_IDLE && KEYDBNC_GetKeys() != 0) {
#if PL_HAS_KBI
		KEY_DisableInterrupts();
#endif
		DBNC_Process(&KEYDBNC_FSMdata);
	}

	/** \todo call DBNC_Scan(&KEYDBNC_FSMdata);
	 * But be careful: only if we are not debouncing, and if we have a key press if we are polling.
	 * And you will need to disable the keyboard interrupts too!
	 */
}

void KEYDBNC_Init(void) {
	/* nothing needed */
}

void KEYDBNC_Deinit(void) {
	/* nothing needed */
}

#endif /* PL_HAS_KEYS && PL_HAS_DEBOUNCE */

