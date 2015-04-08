/**
 * \file
 * \brief Event driver interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a generic event driver. We are using numbered events starting with zero.
 * EVNT_HandleEvent() can be used to process the pending events. Note that the event with the number zero
 * has the highest priority and will be handled first
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Platform.h"

#if PL_HAS_EVENTS

typedef enum EVNT_Handle {
  EVNT_INIT,            /*!< System Initialization Event */
  EVNT_LED_HEARTBEAT,
  EVNT_SW1_PRESSED,
  EVNT_SW2_PRESSED,
  EVNT_SW3_PRESSED,
  EVNT_SW4_PRESSED,
  EVNT_SW5_PRESSED,
  EVNT_SW6_PRESSED,
  EVNT_SW7_PRESSED,
  EVNT_SW1_LPRESSED,
  EVNT_SW2_LPRESSED,
  EVNT_SW3_LPRESSED,
  EVNT_SW4_LPRESSED,
  EVNT_SW5_LPRESSED,
  EVNT_SW6_LPRESSED,
  EVNT_SW7_LPRESSED,
  EVNT_SW1_RELEASED,
  EVNT_SW2_RELEASED,
  EVNT_SW3_RELEASED,
  EVNT_SW4_RELEASED,
  EVNT_SW5_RELEASED,
  EVNT_SW6_RELEASED,
  EVNT_SW7_RELEASED,
  /*!< \todo Your events here */
  EVNT_NOF_EVENTS       /*!< Must be last one! */
} EVNT_Handle;

typedef struct __attribute__((__packed__)) EventAllocation {
	EVNT_Handle handle;
	void (*SpecificHandler)(void);
} EventAllocation; /*!< Used to allocate event handles and handler functions*/

/*!
 * \brief Sets an event.
 * \param[in] event The handle of the event to set.
 */
void EVNT_SetEvent(EVNT_Handle event);

/*!
 * \brief Clears an event.
 * \param[in] event The event handle of the event to clear.
 */
void EVNT_ClearEvent(EVNT_Handle event);

/*!
 * \brief Returns the status of an event.
 * \param[in] event The event handler of the event to check.
 * \return TRUE if the event is set, FALSE otherwise.
 */
bool EVNT_EventIsSet(EVNT_Handle event);

/*!
 * \brief Returns the status of an event. As a side effect, the event gets cleared.
 * \param[in] event The event handler of the event to check.
 * \return TRUE if the event is set, FALSE otherwise.
 */
bool EVNT_EventIsSetAutoClear(EVNT_Handle event);

/*!
 * \brief Routine to check if an event is pending. If an event is pending, the event is cleared and the provided callback is called.
 * \param[in] callback Callback routine to be called. The event handle is passed as argument to the callback.
 */
void EVNT_HandleEvent(EventAllocation* allocation,int allocatonSize);

/*! \brief Event module initialization */
void EVNT_Init(void);

/*! \brief Event module de-initialization */
void EVNT_Deinit(void);

#endif /* PL_HAS_EVENTS */


#endif /* EVENT_H_ */
