/**************************************************************************//**
  \file buttonsExt.h

  \brief
    RCB KeyRemote buttons interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    31.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _BUTTONS_EXTENSION_H
#define _BUTTONS_EXTENSION_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _BSP_ButtonEvent_t
{
  BUTTON_NO_EVENT,
  BUTTON_CHANGED_EVENT,
  BUTTON_PRESSED_EVENT,
  BUTTON_RELEASED_EVENT,
  BUTTON_THRESHOLD_REACHED_EVENT,
  BUTTON_LEVEL_CHANGED_EVENT,
} BSP_ButtonEvent_t;

typedef enum _BSP_ButtonId_t
{
  BUTTON_RED     = 0,
  BUTTON_GREEN   = 1,
  BUTTON_YELLOW  = 2,
  BUTTON_BLUE    = 3,
  BUTTON_SEL     = 4,
  BUTTON_0       = 5,
  BUTTON_PWR     = 6,
  // no button 7
  // no button 8
  BUTTON_1       = 9,
  BUTTON_2       = 10,
  BUTTON_3       = 11,
  BUTTON_4       = 12,
  BUTTON_5       = 13,
  BUTTON_6       = 14,
  BUTTON_7       = 15,
  BUTTON_8       = 16,
  BUTTON_9       = 17,
  BUTTON_UP      = 18,
  BUTTON_LEFT    = 19,
  BUTTON_OK      = 20,
  BUTTON_RIGHT   = 21,
  BUTTON_DOWN    = 22,
  BUTTON_LEFT_P  = 23,
  BUTTON_LEFT_M  = 24,
  BUTTON_RIGHT_P = 25,
  BUTTON_RIGHT_M = 26,

  LAST_BUTTON_INDEX
} BSP_ButtonId_t;

/** Type of buttons event handler */
typedef void (* BSP_ButtonEventHandler_t)(BSP_ButtonEvent_t event, BSP_ButtonId_t button, uint8_t data);

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes buttons handler.

\param[in]
    handler - the handler to process buttons events
\return
  BC_FAIL - buttons module is already used, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_InitButtons(BSP_ButtonEventHandler_t handler);

/**************************************************************************//**
\brief Uninitializes buttons handler.
\return
  BC_FAIL - buttons module was not opened, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_UninitButtons(void);

/**************************************************************************//**
\brief Set threshold value for a button
\param[in] button - button index
\param[in] threshold - value to set
******************************************************************************/
void BSP_SetButtonThreshold(BSP_ButtonId_t button, uint8_t threshold);

/**************************************************************************//**
\brief Determine if specified button is currently pressed.
\param[in] button - button index
\returns true - button is pressed
\returns false - button is released
******************************************************************************/
bool BSP_IsButtonPressed(BSP_ButtonId_t button);

#endif // _BUTTONS_EXTENSION_H
// eof buttons.h
