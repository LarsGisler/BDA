/**
 * \file
 * \brief Key/Switch driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a generic keyboard driver for up to 7 Keys.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#include "Platform.h"
#if PL_HAS_KEYS
#include "Keys.h"
#if PL_HAS_EVENTS
#include "Event.h"
#endif
#if PL_HAS_DEBOUNCE
#include "KeyDebounce.h"
#endif
#include "cpu.h"




void KEY_Scan(void) {
#if PL_HAS_DEBOUNCE
	KEYDBNC_Scan();
#else
	/*! \todo check handling all keys */
#if PL_NOF_KEYS >= 1 && PL_KEY_POLLED_KEY1
	if (KEY1_Get()) { /* key pressed */
		EVEVNT_SetEvent(EVNT_SW1_PRESSED);
	}
#endif
#endif
}

#if PL_HAS_KBI
void KEY_OnInterrupt(KEY_Buttons button) {
#if PL_HAS_DEBOUNCE
	KEYDBNC_Scan();
#else
	switch (button) {
#if PL_NOF_KEYS >= 1
		case KEY_BTN1:
		EVNT_SetEvent(EVNT_SW1_PRESSED);
		break;
#endif
		default:
		/* unknown? */
		break;
	} /* switch */
#endif
}

void KEY_EnableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
	SW1_Enable();
#endif
#if PL_HAS_KBI_NMI
	/* necessary to clear interrupts on Port A */
	PORT_PDD_ClearPinInterruptFlag(PORTA_BASE_PTR, ExtIntLdd4_PIN_INDEX);
	PORT_PDD_SetPinInterruptConfiguration(PORTA_BASE_PTR, 4, PORT_PDD_DMA_ON_FALLING);
#endif
}

void KEY_DisableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
	SW1_Disable();
#endif
#if PL_HAS_KBI_NMI
	PORT_PDD_SetPinInterruptConfiguration(PORTA_BASE_PTR, 4, PORT_PDD_INTERRUPT_DMA_DISABLED);
#endif
	//PORT_PDD_ClearPinInterruptFlag(PORTA_BASE_PTR, ExtIntLdd3_PIN_INDEX);
}

void PORTA_OnInterrupt(void) {
	void Cpu_ivINT_PORTA(void); /* prototype of ISR in Cpu.c */

	Cpu_ivINT_PORTA(); /* call interrupt handler created by the ExtInt components */
}
#endif

/*! \brief Key driver initialization */
void KEY_Init(void) {
	/* nothing needed for now */
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
	/* nothing needed for now */
}
#endif /* PL_HAS_KEYS */
