/**
 * \file
 * \brief provides pre-configuration of the program
 * \author Lars Gisler
 *
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "PE_Types.h" /* for common Processor Expert types used throughout the project, e.g. 'bool' */
#include "PE_Error.h" /* global error constants */

/* List of supported platforms. The PL_BOARD_IS_xxx is defined in the compiler command line settings.  */
#define PL_IS_FRDM   		(1)

#define PL_HAS_LED      	(1)
#define PL_HAS_HAS_TIMER 	(1)
#define PL_HAS_EVENTS		(1)
#define PL_HAS_TRIGGER		(1)
#define PL_HAS_RTOS			(1)
#define PL_HAS_USB_CDC		(1)
#define PL_NOF_LEDS       	(3)
#define PL_HAS_SENSOR		(1) /* set to 0 to load dummy data */
#define PL_HAS_COM			(1)
#define PL_HAS_SERVICE_PROTOCOL	(1)
#define PL_HAS_REPEAT_PROTOCOL	(0)


void PL_Init(void);
void PL_Deinit(void);

#endif /* PLATFORM_H_ */
