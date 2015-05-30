/**
 * \file
 * \brief LED driver implementation
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * modified by Lars Gisler
 */

#ifndef LED_H_
#define LED_H_
#include "Platform.h"

#if PL_NOF_LEDS >= 1
  #include "Led1.h"
  #define LED1_On()     Led1_ClrVal()
  #define LED1_Off()    Led1_SetVal()
  #define LED1_Neg()    Led1_NegVal()
  #define LED1_Get()    Led1_GetVal()
  #define LED1_Put(val) Led1_PutVal(!val)
  #define LED1_Init()   /* do nothing */
  #define LED1_Deinit() /* do nothing */
#else
  #define LED1_On()     /* do nothing */
  #define LED1_Off()    /* do nothing */
  #define LED1_Neg()    /* do nothing */
  #define LED1_Get()  0 /* do nothing */
  #define LED1_Put(val) /* do nothing */
  #define LED1_Init()   /* do nothing */
  #define LED1_Deinit() /* do nothing */
#endif

#if PL_NOF_LEDS >= 2
  #include "Led2.h"
  #define LED2_On()     Led2_ClrVal()
  #define LED2_Off()    Led2_SetVal()
  #define LED2_Neg()    Led2_NegVal()
  #define LED2_Get()    Led2_GetVal()
  #define LED2_Put(val) Led2_PutVal(!val)
  #define LED2_Init()   /* do nothing */
  #define LED2_Deinit() /* do nothing */
#else
  #define LED2_On()     /* do nothing */
  #define LED2_Off()    /* do nothing */
  #define LED2_Neg()    /* do nothing */
  #define LED2_Get()  0 /* do nothing */
  #define LED2_Put(val) /* do nothing */
  #define LED2_Init()   /* do nothing */
  #define LED2_Deinit() /* do nothing */
#endif

#if PL_NOF_LEDS >= 3
  #include "Led3.h"
  #define LED3_On()     Led3_ClrVal()
  #define LED3_Off()    Led3_SetVal()
  #define LED3_Neg()    Led3_NegVal()
  #define LED3_Get()    Led3_GetVal()
  #define LED3_Put(val) Led3_PutVal(!val)
  #define LED3_Init()   /* do nothing */
  #define LED3_Deinit() /* do nothing */
#else
  #define LED3_On()     /* do nothing */
  #define LED3_Off()    /* do nothing */
  #define LED3_Neg()    /* do nothing */
  #define LED3_Get()  0 /* do nothing */
  #define LED3_Put(val) /* do nothing */
  #define LED3_Init()   /* do nothing */
  #define LED3_Deinit() /* do nothing */
#endif

void LED2m_Off();
void LED3m_Off();

void LED_Init(void);
void LED_Deinit(void);

#endif /* LED_H_ */
