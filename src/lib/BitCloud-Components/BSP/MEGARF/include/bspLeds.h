/***************************************************************************//**
\file  bspLeds.h

\brief Declaration of leds defines.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    26.08.09 A. Taradov - Created
*******************************************************************************/

#ifndef _BSPLEDS_H
#define _BSPLEDS_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <gpio.h>
#include <leds.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/

#if (BSP_SUPPORT == BOARD_RCB) || (BSP_SUPPORT == BOARD_RCB_KEY_REMOTE)
  // First LED
  #define halInitFirstLed()       GPIO_E2_make_out()
  #define halUnInitFirstLed()     GPIO_E2_make_in()
  #define halOnFirstLed()         GPIO_E2_clr()
  #define halOffFirstLed()        GPIO_E2_set()
  #define halReadFirstLed()       GPIO_E2_read()
  #define halToggleFirstLed()     GPIO_E2_toggle()
  // Second LED
  #define halInitSecondLed()      GPIO_E3_make_out()
  #define halUnInitSecondLed()    GPIO_E3_make_in()
  #define halOnSecondLed()        GPIO_E3_clr()
  #define halOffSecondLed()       GPIO_E3_set()
  #define halReadSecondLed()      GPIO_E3_read()
  #define halToggleSecondLed()    GPIO_E3_toggle()
  #if (APP_INTERFACE == APP_INTERFACE_USBFIFO)
    // Third LED
    #define halInitThirdLed()
    #define halUnInitThirdLed()
    #define halOnThirdLed()
    #define halOffThirdLed()
    #define halReadThirdLed()
    #define halToggleThirdLed()
  #else
    #define halInitThirdLed()       GPIO_E4_make_out()
    #define halUnInitThirdLed()     GPIO_E4_make_in()
    #define halOnThirdLed()         GPIO_E4_clr()
    #define halOffThirdLed()        GPIO_E4_set()
    #define halReadThirdLed()       GPIO_E4_read()
    #define halToggleThirdLed()     GPIO_E4_toggle()
  #endif // APP_INTERFACE == APP_INTERFACE_USBFIFO
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  #define halInitFirstLed()       GPIO_D6_make_out()
  #define halUnInitFirstLed()     GPIO_D6_make_in()
  #define halOnFirstLed()         GPIO_D6_clr()
  #define halOffFirstLed()        GPIO_D6_set()
  #define halReadFirstLed()       GPIO_D6_read()
  #define halToggleFirstLed()     GPIO_D6_toggle()

  #define halInitSecondLed()       GPIO_G2_make_out()
  #define halUnInitSecondLed()     GPIO_G2_make_in()
  #define halOnSecondLed()         GPIO_G2_clr()
  #define halOffSecondLed()        GPIO_G2_set()
  #define halReadSecondLed()       GPIO_G2_read()
  #define halToggleSecondLed()     GPIO_G2_toggle()

  #define halInitThirdLed()       GPIO_E2_make_out()
  #define halUnInitThirdLed()     GPIO_E2_make_in()
  #define halOnThirdLed()         GPIO_E2_clr()
  #define halOffThirdLed()        GPIO_E2_set()
  #define halReadThirdLed()       GPIO_E2_read()
  #define halToggleThirdLed()     GPIO_E2_toggle()

#endif // BSP_SUPPORT
#endif /*_BSPLEDS_H*/
// eof bspLeds.h
