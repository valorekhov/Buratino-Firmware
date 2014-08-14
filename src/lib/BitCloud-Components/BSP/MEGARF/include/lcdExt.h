/**************************************************************************//**
  \file lcdExt.h

  \brief
    S32128A (ST7565V) Additional LCD routines

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.08.11 A. Taradov - Created.
******************************************************************************/

#ifndef _LCD_EXTENSION_H_
#define _LCD_EXTENSION_H_

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Clear screen
******************************************************************************/
void BSP_LcdCls(void);

/**************************************************************************//**
\brief Print a formatted string on the LCD
******************************************************************************/
void BSP_LcdPrint(uint8_t x, uint8_t y, const char *format, ...);

/**************************************************************************//**
\brief Fill the LCD screen (using internal test command)
******************************************************************************/
void BSP_LcdFill(bool fill);

/**************************************************************************//**
\brief Initialize the LCD
******************************************************************************/
void BSP_LcdInit(void);

#endif // _LCD_H_

// eof lcd.h
