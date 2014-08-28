/***************************************************************************//**
\file  leds.c

\brief The module to access to the leds.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    05.08.09 A. Taradov - Created
*******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE

/******************************************************************************
                   Includes section
******************************************************************************/
#include <bspLeds.h>

#if BSP_SUPPORT == BOARD_RCB_KEY_REMOTE
#include <leds.h>
#include <sysTypes.h>
#include <atomic.h>
#include <appTimer.h>
#include <hs2xy.h>
#include <maxL.h>
#include <xyl2rgb.h>
#include <dithPwm.h>
#else
#include <leds.h>
#include <pwm.h>
#include <appTimer.h>
#endif // BSP_SUPPORT == BOARD_RCB_KEY_REMOTE

/******************************************************************************
                    Definitions section
******************************************************************************/
#define STORE_BUTTONS_REGS {         \
  uint8_t eimsk = EIMSK;             \
  EIMSK = 0;                         \
  uint8_t ddrb = DDRB;               \
  uint8_t portb = PORTB;             \
  DDRB = 0x1f; // Data outputs

#define RECOVER_BUTTONS_REGS         \
  DDRB = ddrb;                       \
  PORTB = portb;                     \
  asm("nop");                        \
  EIMSK = eimsk; }

#define  LED_SEL              (1 << PE5)

/* Maximum duty cycle (%) */
#define APP_MAX_DUTY_CYCLE 100

/* Minimum duty cycle (%) */
#define APP_MIN_DUTY_CYCLE 0

/* Base frequency in Hz */
#define APP_BASE_FREQUENCY 900

/* Macro for top counter value calculation */
#define GET_TOP(freq, prescale)   (F_CPU / (2 * freq * prescale))
/* Use prescale factor 1 */
#define PRESCALER  PWM_PRESCALER_1
/* Set top counter value using base frequency of 10kHz */
#define TOP GET_TOP(APP_BASE_FREQUENCY, PRESCALER)

/******************************************************************************
                    Prototypes section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

#if defined(APP_USE_PWM)
static void initPwm(void);
static void pwmSetDutyCycle(HAL_PwmDescriptor_t *descriptor, uint8_t newDutyCycle);
#endif // APP_USE_PWM

#else // BOARD_RCB_KEY_REMOTE

#if defined(APP_USE_PWM)
static void ledsSetColor(void);

/******************************************************************************
                    Local variables
******************************************************************************/
static uint16_t currentX, currentY;
#endif /* APP_USE_PWM */

static uint8_t portValue;
#endif // BOARD_RCB_KEY_REMOTE

#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE && defined(APP_USE_PWM)
static HAL_PwmDescriptor_t pwmChannel0;
#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE && defined(APP_USE_PWM)

#if defined(APP_USE_PWM)
static uint8_t currentL;
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE
/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
void BSP_InitLeds(void)
{
  halInitFirstLed();
  #if BSP_SUPPORT == BOARD_RCB
  halInitSecondLed();
  halInitThirdLed();
  #endif // BSP_SUPPORT == BOARD_RCB

#if defined(APP_USE_PWM)
  initPwm();
  (void)currentL;
#endif // APP_USE_PWM

  BSP_OffLed(LED_FIRST);
  #if BSP_SUPPORT == BOARD_RCB
  BSP_OffLed(LED_SECOND);
  BSP_OffLed(LED_THIRD);
  #endif // BSP_SUPPORT == BOARD_RCB
}

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  BSP_InitLeds();
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Closes leds module.

\return
    operation state
******************************************************************************/
result_t BSP_CloseLeds(void)
{
  halUnInitFirstLed();
  #if BSP_SUPPORT == BOARD_RCB
  halUnInitSecondLed();
  halUnInitThirdLed();
  #endif // BSP_SUPPORT == BOARD_RCB
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Turns the LED on.

\param[in]
    id - number of led
******************************************************************************/
void BSP_OnLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halOnFirstLed();
      break;
#if BSP_SUPPORT == BOARD_RCB
    case LED_SECOND:
      halOnSecondLed();
      break;
    case LED_THIRD:
#if defined(APP_USE_PWM)
      currentL = 255;
      pwmSetDutyCycle(&pwmChannel0, APP_MAX_DUTY_CYCLE);
#else
      halOnThirdLed();
#endif // APP_USE_PWM
      break;
#endif // BSP_SUPPORT == BOARD_RCB
    default:
      break;
  }
}

/**************************************************************************//**
\brief Turns the LED off.

\param[in]
      id - number of led
******************************************************************************/
void BSP_OffLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halOffFirstLed();
      break;
#if BSP_SUPPORT == BOARD_RCB
    case LED_SECOND:
      halOffSecondLed();
      break;
    case LED_THIRD:
#if defined(APP_USE_PWM)
      currentL = 0;
      pwmSetDutyCycle(&pwmChannel0, APP_MIN_DUTY_CYCLE);
#else
      halOffThirdLed();
#endif // APP_USE_PWM
      break;

#endif // BSP_SUPPORT == BOARD_RCB
    default:
      break;
  }
}

/**************************************************************************//**
\brief Toggles LED state.

\param[in]
      id - number of led
******************************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halToggleFirstLed();
      break;
#if BSP_SUPPORT == BOARD_RCB
    case LED_SECOND:
      halToggleSecondLed();
      break;
    case LED_THIRD:
#if defined(APP_USE_PWM)
      pwmSetDutyCycle(&pwmChannel0, currentL ? APP_MIN_DUTY_CYCLE : APP_MAX_DUTY_CYCLE);
      currentL = currentL ? 0 : 255;
#else
      halToggleThirdLed();
#endif
      break;

#endif // BSP_SUPPORT == BOARD_RCB
    default:
      break;
  }
}

#if defined(APP_USE_PWM)
/**************************************************************************//**
\brief Supported PWM channels initialization.
******************************************************************************/
static void initPwm(void)
{
  /* Descriptors initialization */
  pwmChannel0.unit = PWM_UNIT_3;
#if (APP_INTERFACE == APP_INTERFACE_USBFIFO)  
  pwmChannel0.channel = PWM_CHANNEL_0;
#else
  pwmChannel0.channel = PWM_CHANNEL_1; 
#endif  
  pwmChannel0.polarity = PWM_POLARITY_NON_INVERTED;

  /* Prepare required PWM channels */
  HAL_OpenPwm(PWM_UNIT_3);
  /* Set PWM frequency for required module */
  HAL_SetPwmFrequency(PWM_UNIT_3, TOP, PRESCALER);

  /* Start PWM on supported channels */
  HAL_StartPwm(&pwmChannel0);
}

/**************************************************************************//**
\brief Duty cycle calculation and setting.
\param [in] descriptor - PWM channel descriptor.
\param [in] newDutyCycle - new duty cycle value (0-100 %)
******************************************************************************/
static void pwmSetDutyCycle(HAL_PwmDescriptor_t *descriptor, uint8_t newDutyCycle)
{
  /* Calculate new duty cycle */
  uint16_t cmpValue = (TOP * newDutyCycle) / 100;
  /* Set new duty cycle */
  HAL_SetPwmCompareValue(descriptor, cmpValue);
}
#endif /* APP_USE_PWM */

/**************************************************************************//**
\brief Set brightness level
\param[in] level - new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level)
{
#ifdef APP_USE_PWM
  currentL = level;
  pwmSetDutyCycle(&pwmChannel0, ((uint16_t)level * 100) / 255);
#else
  /* for RCB and Xplained_PRO boards */
  uint8_t id = LED_THIRD;
  #if BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO || BSP_SUPPORT == BOARD_BURATINO
    id = LED_FIRST;
  #endif
  if (level)
    BSP_OnLed(id);
  else
    BSP_OffLed(id);
#endif
 (void)level;
}

#else // BOARD_RCB_KEY_REMOTE
/**************************************************************************//**
\brief LEDs initialization.
******************************************************************************/
void BSP_InitLeds(void)
{
#if defined(APP_USE_PWM)
  currentL = 0;
#endif

#ifdef APP_USE_PWM
  currentX = currentY = 32767;

  dithStartTimer();
  dithSetFrequency(APP_PWM_FREQUENCY);
  dithSetDutyCycles(0, 0, 0);
  dithSetDithering(ENABLE_DITHERING_A, ENABLE_DITHERING_B, ENABLE_DITHERING_C);
#endif
}

/**************************************************************************//**
\brief Set the brightness level
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level)
{
#if defined(APP_USE_PWM)
  currentL = level;
  ledsSetColor();
#endif /* APP_USE_PWM */
  (void)level;
}

/**************************************************************************//**
\brief Set the color using Hue and Saturation as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorHS(uint16_t hue, uint8_t saturation)
{
#ifdef APP_USE_PWM
  unsigned int x, y;

  HS2XY(hue >> 8, saturation, &x, &y);
  currentX = x;
  currentY = y;
  ledsSetColor();
#else
  (void)hue, (void)saturation;
#endif
}

/**************************************************************************//**
\brief Set the color using X and Y as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorXY(uint16_t x, uint16_t y)
{
#ifdef APP_USE_PWM
  currentX = x;
  currentY = y;
  ledsSetColor();
#else
  (void)x, (void)y;
#endif
}

/**************************************************************************//**
\brief Set PWM values for current X, Y and L values
******************************************************************************/
#if defined(APP_USE_PWM)
static void ledsSetColor(void)
{
#ifdef APP_USE_PWM
  int maxL;
  unsigned int r, g, b;

  maxL_FromXY(currentX, currentY, &maxL);
  XYL2RGB(currentX, currentY, currentL, maxL, &r, &g, &b);

  dithSetDutyCycles(r, g, b);
#endif
}
#endif /* APP_USE_PWM */

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  STORE_BUTTONS_REGS
  DDRE |= LED_SEL;
  PORTE &= ~LED_SEL;
  PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2) |
          (1 << PB3) | (1 << PB4);
  portValue = PORTB;
  PORTE |= LED_SEL;
  asm("nop");
  PORTE &= ~LED_SEL;
  RECOVER_BUTTONS_REGS
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Closes leds module.

\return
    operation state
******************************************************************************/
result_t BSP_CloseLeds(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Turns the LED on.

\param[in]
    id - number of led
******************************************************************************/
void BSP_OnLed(uint8_t id)
{
  STORE_BUTTONS_REGS
  PORTB = portValue & ~(1 << id);
  portValue = PORTB;
  PORTE |= LED_SEL;
  asm("nop");
  PORTE &= ~LED_SEL;
  RECOVER_BUTTONS_REGS
}

/**************************************************************************//**
\brief Turns the LED off.

\param[in]
      id - number of led
******************************************************************************/
void BSP_OffLed(uint8_t id)
{
  STORE_BUTTONS_REGS
  PORTB = portValue | (1 << id);
  portValue = PORTB;
  PORTE |= LED_SEL;
  asm("nop");
  PORTE &= ~LED_SEL;
  RECOVER_BUTTONS_REGS
}

/**************************************************************************//**
\brief Toggles LED state.

\param[in]
      id - number of led
******************************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  STORE_BUTTONS_REGS
  PORTB = portValue ^ (1 << id);
  portValue = PORTB;
  PORTE |= LED_SEL;
  asm("nop");
  PORTE &= ~LED_SEL;
  RECOVER_BUTTONS_REGS
}

#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

#endif // BSP_SUPPORT != BOARD_FAKE

// eof leds.c
