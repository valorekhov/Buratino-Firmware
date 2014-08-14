/**************************************************************************//**
  \file lcd.c

  \brief
    S32128A (ST7565V) LCD routines implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.08.11 A. Taradov - Created.
******************************************************************************/
#if BSP_SUPPORT == BOARD_RCB_KEY_REMOTE

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <lcd.h>
#include <lcdExt.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define nCS1   (1 << PE4)
#define nRES   (1 << PE2)
#define A0     (1 << PE3)
#define RW     (1 << PE6)
#define E      (1 << PE7)

#define NUMBER_OF_LCD_BANKS     8
#define NUMBER_OF_LCD_COLUMNS   132 // 128+4 due to different memory arrangement when mirroring is enabled

#define NUMBER_OF_TEXT_LINES    4
#define NUMBER_OF_TEXT_COLUMNS  21

#define STORE_BUTTONS_REGS {         \
  uint8_t eimsk = EIMSK;             \
  EIMSK = 0;                         \
  uint8_t ddrb = DDRB;               \
  uint8_t portb = PORTB;             \
  DDRB = 0xff; // Data outputs

#define RECOVER_BUTTONS_REGS         \
  DDRB = ddrb;                       \
  PORTB = portb;                     \
  asm("nop");                        \
  EIMSK = eimsk; }

/******************************************************************************
                    Local variables section
******************************************************************************/
static uint8_t cursorx, cursory;

static uint8_t PROGMEM_DECLARE(fontdata[]) =
{
  0x00, 0x00, 0x00, 0x00, 0x00, // (space)    #32
  0x00, 0x00, 0x5F, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7F, 0x14, 0x7F, 0x14, // #
  0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
  0x23, 0x13, 0x08, 0x64, 0x62, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x1C, 0x22, 0x41, 0x00, // (          #40
  0x00, 0x41, 0x22, 0x1C, 0x00, // )
  0x08, 0x2A, 0x1C, 0x2A, 0x08, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x50, 0x30, 0x00, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 2          #50
  0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x00, 0x08, 0x14, 0x22, 0x41, // <          #60
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x41, 0x22, 0x14, 0x08, 0x00, // >
  0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x32, 0x49, 0x79, 0x41, 0x3E, // @
  0x7E, 0x11, 0x11, 0x11, 0x7E, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x01, 0x01, // F          #70
  0x3E, 0x41, 0x41, 0x51, 0x32, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x04, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P          #80
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x7F, 0x20, 0x18, 0x20, 0x7F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x51, 0x49, 0x45, 0x43, // Z          #90
  0x00, 0x00, 0x7F, 0x41, 0x41, // [
  0x02, 0x04, 0x08, 0x10, 0x20, // "\"
  0x41, 0x41, 0x7F, 0x00, 0x00, // ]
  0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x01, 0x02, 0x04, 0x00, // `
  0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x38, 0x44, 0x44, 0x48, 0x7F, // d         #100
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x08, 0x14, 0x54, 0x54, 0x3C, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x44, 0x3D, 0x00, // j
  0x00, 0x7F, 0x10, 0x28, 0x44, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n         #110
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0x7C, 0x14, 0x14, 0x14, 0x08, // p
  0x08, 0x14, 0x14, 0x18, 0x7C, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x         #120
  0x0C, 0x50, 0x50, 0x50, 0x3C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00, // {
  0x00, 0x00, 0x7F, 0x00, 0x00, // |
  0x00, 0x41, 0x36, 0x08, 0x00, // }
  0x02, 0x01, 0x02, 0x04, 0x02, // ~
  0x78, 0x44, 0x42, 0x44, 0x78  // DEL
};

/******************************************************************************
                    Prototypes section
******************************************************************************/
void HAL_Delay(uint8_t us);

/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Send command to the LCD
******************************************************************************/
static void lcdCommand(uint8_t command)
{
  STORE_BUTTONS_REGS
  PORTB = command;

  PORTE |= E;
  PORTE &= ~A0;
  PORTE &= ~RW;

  PORTE &= ~nCS1;
  asm("nop"); // HAL_Delay(1);
  PORTE |= nCS1;

  PORTE |= RW;
  PORTE &= ~E;
  RECOVER_BUTTONS_REGS
}

/**************************************************************************//**
\brief Send data to the LCD
******************************************************************************/
static void lcdData(uint8_t data)
{
  STORE_BUTTONS_REGS
  PORTB = data;

  PORTE |= E;
  PORTE |= A0;
  PORTE &= ~RW;

  PORTE &= ~nCS1;
  asm("nop"); // HAL_Delay(1);
  PORTE |= nCS1;

  PORTE |= RW;
  PORTE &= ~E;
  RECOVER_BUTTONS_REGS
}

/**************************************************************************//**
\brief Print a character on the LCD at current position
******************************************************************************/
static void lcdChar(uint8_t chr)
{
  uint8_t font[5];
  uint16_t offset;
  uint8_t column = (cursorx+1)*6 - 1; // complex formula due to all the mirroring done by the LCD

  lcdCommand(0x00 | (column & 0x0f));
  lcdCommand(0x10 | ((column >> 4) & 0x0f));
  lcdCommand(0xb0 | (cursory & 0x03));

  if (chr < 32 || chr > 127)
    chr = '.';

  offset = (chr-32) * 5;
  memcpy_P(font, &fontdata[offset], sizeof(font));

  for (uint8_t i = 0; i < 5; i++)
    lcdData(font[i]);

  lcdData(0); // space between characters

  cursorx++;
  if (cursorx == NUMBER_OF_TEXT_COLUMNS)
  {
    cursorx = 0;
    cursory++;
  }

  if (cursory > (NUMBER_OF_TEXT_COLUMNS-1))
    cursory = 0;
}

/**************************************************************************//**
\brief Clear screen
******************************************************************************/
void BSP_LcdCls(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LCD_BANKS; i++)
  {
    lcdCommand(0x00);
    lcdCommand(0x10);
    lcdCommand(0xb0 | i);

    for (uint8_t j = 0; j < NUMBER_OF_LCD_COLUMNS; j++)
      lcdData(0x00);
  }
}

/**************************************************************************//**
\brief Fill the LCD screen (using internal test command)
******************************************************************************/
void BSP_LcdFill(bool fill)
{
//  lcdCommand(0xa4 | (fill ? 1 : 0));
  lcdCommand(0xa6 | (fill ? 1 : 0));

  // HACK! This portion of magic will switch off the LED conflicting with LCD.
  if (!fill)
  {
    BSP_LcdPrint(0, 3, " ");
  }
}

/**************************************************************************//**
\brief Print a formatted string on the LCD
******************************************************************************/
void BSP_LcdPrint(uint8_t x, uint8_t y, const char *format, ...)
{
  uint8_t str[100];
  char *ptr = (char *)str;
  uint8_t len;
  va_list ap;

  cursorx = x;
  cursory = y;

  va_start(ap, format);
  len = vsnprintf(ptr, 100, format, ap);
  va_end(ap);
  while (len--)
    lcdChar(*ptr++);
}

/**************************************************************************//**
\brief Initialize the LCD
******************************************************************************/
void BSP_LcdInit(void)
{
  DDRE |= nCS1 | nRES | A0 | RW | E; // Control outputs
  PORTE |= nCS1;

  PORTE &= ~nRES;
  HAL_Delay(200);
  HAL_Delay(200);
  HAL_Delay(200);
  PORTE |= nRES;

  for (uint8_t i = 0; i < 50; i++) // delay of 10ms
    HAL_Delay(200);

  lcdCommand(0x2f); // Power controller: booster on, voltage regulator on, voltage follower on
  lcdCommand(0xa2); // 1/6 bias
  lcdCommand(0xa1); // ADC select: normal
  lcdCommand(0xc2); // Common output mode: normal
  lcdCommand(0x22); // Voltage regulator internal resistor ratio
  lcdCommand(0x81); // Volume mode
  lcdCommand(0x25); // Volume
  lcdCommand(0x40); // Display start line: 0
  lcdCommand(0xa6); // Display normal
  lcdCommand(0xaf); // Display on

  BSP_LcdCls();
}
#endif // BSP_SUPPORT == BOARD_RCB_KEY_REMOTE
// eof lcd.c
