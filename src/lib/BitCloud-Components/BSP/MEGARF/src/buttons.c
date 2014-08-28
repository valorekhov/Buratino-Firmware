/**************************************************************************//**
\file  buttons.c

\brief Implementation of buttons interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    21.08.09 A. Taradov - Created
*******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <buttons.h>
#include <irq.h>
#include <appTimer.h>
#include <bspTaskManager.h>
#include <gpio.h>
#include <bspDbg.h>
#include <buttonsExt.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if BSP_SUPPORT == BOARD_RCB
  #define BSP_readKEY0()    GPIO_E5_read()
  #define PRESSED           1
  #define RELEASED          0
  #define BSP_BUTTONS_IDLE  0
  #define BSP_BUTTONS_BUSY  1
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO || BSP_SUPPORT == BOARD_BURATINO
  #define BSP_readKEY0()    GPIO_E4_read()
  #define PRESSED           1
  #define RELEASED          0
  #define BSP_BUTTONS_IDLE  0
  #define BSP_BUTTONS_BUSY  1
#elif BSP_SUPPORT == BOARD_RCB_KEY_REMOTE
  #define N_ROWS               3
  #define N_COLUMNS            9
  #define N_BUTTONS            (N_ROWS * N_COLUMNS)
  #define SCAN_TIMER_INTERVAL  100
#endif // BSP_SUPPORT

#define STORE_BUTTONS_REGS {         \
  uint8_t eimsk = EIMSK;             \
  EIMSK = 0;                         \
  uint8_t ddrb = DDRB;               \
  uint8_t portb = PORTB;             \
  uint8_t ddrd = DDRD;               \
  uint8_t portd = PORTD;             \

#define RECOVER_BUTTONS_REGS         \
  DDRB = ddrb;                       \
  PORTB = portb;                     \
  DDRD = ddrd;                       \
  PORTD = portd;                     \
  asm("nop");                        \
  EIMSK = eimsk; }

/******************************************************************************
                   Types section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE
typedef struct
{
  uint8_t currentState0 : 1;
  uint8_t wasPressed0   : 1;
  uint8_t waitReleased0 : 1;
} BSP_buttonsAction_t;
#else
typedef struct _Button_t
{
  bool          pressed;
  BSP_ButtonEvent_t event;
  uint8_t       interval;
  uint8_t       threshold;
} Button_t;

typedef enum _ButtonsState_t
{
  BSP_BUTTONS_IDLE = 0,
  BSP_BUTTONS_BUSY = 1,
} ButtonsState_t;
#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

/******************************************************************************
                   Prototypes section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE
/**************************************************************************//**
\brief  HAL's event handlers about KEY 0 has changed state.
******************************************************************************/
void bspKey0InterruptHandler(void);
#else
static void bspInitButtons(void);
static void bspUninitButtons(void);
static void keyboardInterruptHandler(void);
static void scanTimerFired(void);
static void enableIrqs(void);
static void disableIrqs(void);
void __delay_us(uint8_t us);
#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

/******************************************************************************
                   Global variables section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE
static uint8_t state = BSP_BUTTONS_IDLE;
static volatile BSP_buttonsAction_t buttonsAction;
static BSP_ButtonsEventFunc_t bspButtonPressHandle;   // callback
static BSP_ButtonsEventFunc_t bspButtonReleaseHandle; // callback
#else
static ButtonsState_t state = BSP_BUTTONS_IDLE;
static Button_t buttons[N_BUTTONS];
static HAL_AppTimer_t scanTimer;
static bool interrupt = false;
static BSP_ButtonEventHandler_t bspButtonEventHandler = NULL;
#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

/******************************************************************************
                   Implementations section
******************************************************************************/
#if BSP_SUPPORT != BOARD_RCB_KEY_REMOTE
/**************************************************************************//**
\brief Initializes buttons module.
******************************************************************************/
static void bspInitButtons(void)
{
#if BSP_SUPPORT == BOARD_RCB
  GPIO_E5_make_in();
  GPIO_E5_make_pullup();
  HAL_RegisterIrq(IRQ_5, IRQ_LOW_LEVEL, bspKey0InterruptHandler);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO  
  GPIO_E4_make_in();
  GPIO_E4_make_pullup();
  HAL_RegisterIrq(IRQ_4, IRQ_LOW_LEVEL, bspKey0InterruptHandler);  
#endif
  
  if (BSP_readKEY0())
    buttonsAction.currentState0 = RELEASED;
  else
    buttonsAction.currentState0 = PRESSED;
    
#if BSP_SUPPORT == BOARD_RCB
  HAL_EnableIrq(IRQ_5);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  HAL_EnableIrq(IRQ_4);
#endif
}
/**************************************************************************//**
\brief UnInitializes buttons module.
******************************************************************************/
static void bspUnInitButtons(void)
{
#if BSP_SUPPORT == BOARD_RCB
  GPIO_E5_make_out();
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  GPIO_E4_make_out();
#endif

#if BSP_SUPPORT == BOARD_RCB
  HAL_DisableIrq(IRQ_5);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  HAL_DisableIrq(IRQ_4);
#endif
}

/**************************************************************************//**
\brief Registers handlers for button events.

\param[in]
    pressed - the handler to process pressing the button
\param[in]
    released - the handler to process releasing the button
\param[in]
    bn - button number.
\return
  BC_FAIL - buttons module is busy, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn))
{
  if (state != BSP_BUTTONS_IDLE)
    return BC_FAIL;
  state = BSP_BUTTONS_BUSY;
  bspButtonPressHandle = pressed;
  bspButtonReleaseHandle = released;
  bspInitButtons();
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Cancel buttons handlers.
\return
  BC_FAIL - buttons module was not opened, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_CloseButtons(void)
{
  if (state != BSP_BUTTONS_BUSY)
    return BC_FAIL;
#if BSP_SUPPORT == BOARD_RCB
  HAL_UnregisterIrq(IRQ_5);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  HAL_UnregisterIrq(IRQ_4);
#endif
  bspButtonPressHandle = NULL;
  bspButtonReleaseHandle = NULL;
  state = BSP_BUTTONS_IDLE;
  bspUnInitButtons();
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Reads state of buttons.

\return
    Current buttons state in a binary way. \n
    Bit 0 defines state of the button 1, \n
    bit 1 defines state of the button 2.
******************************************************************************/
uint8_t BSP_ReadButtonsState(void)
{
  uint8_t state = 0;

  if (buttonsAction.currentState0)
    state = 0x01;

  return state;
}

/**************************************************************************//**
\brief  HAL's event about KEY has changed state.
******************************************************************************/
void bspKey0InterruptHandler(void)
{
#if BSP_SUPPORT == BOARD_RCB
  HAL_DisableIrq(IRQ_5);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
  HAL_DisableIrq(IRQ_4);
#endif
  buttonsAction.currentState0 = PRESSED;
  buttonsAction.wasPressed0 = 1;
  bspPostTask(BSP_BUTTONS);
}

/**************************************************************************//**
\brief  BSP's event about KEY has changed state.
******************************************************************************/
void bspButtonsHandler(void)
{
  if (buttonsAction.wasPressed0)
  {
    buttonsAction.wasPressed0 = 0;
    buttonsAction.waitReleased0 = 1;
    if (NULL != bspButtonPressHandle)
      bspButtonPressHandle(BSP_KEY0);
  }

  if (buttonsAction.waitReleased0)
  {
    if (BSP_readKEY0())
    {
      buttonsAction.waitReleased0 = 0;
      buttonsAction.currentState0 = RELEASED;
      if (NULL != bspButtonReleaseHandle)
        bspButtonReleaseHandle(BSP_KEY0);
#if BSP_SUPPORT == BOARD_RCB
      HAL_EnableIrq(IRQ_5);
#elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
      HAL_EnableIrq(IRQ_4);
#endif
    }
    else
    {
      bspPostTask(BSP_BUTTONS);
    }
  }
}
#else // BOARD_RCB_KEY_REMOTE
/**************************************************************************//**
\brief Initializes buttons handler.

\param[in]
    handler - the handler to process buttons events
\return
  BC_FAIL - buttons module is already used, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_InitButtons(BSP_ButtonEventHandler_t handler)
{
  if (state != BSP_BUTTONS_IDLE)
    return BC_FAIL;
  state = BSP_BUTTONS_BUSY;
  bspButtonEventHandler = handler;
  bspInitButtons();
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Uninitializes buttons handler.
\return
  BC_FAIL - buttons module was not opened, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_UninitButtons(void)
{
  if (state != BSP_BUTTONS_BUSY)
    return BC_FAIL;
  bspUninitButtons();
  bspButtonEventHandler = NULL;
  state = BSP_BUTTONS_IDLE;
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Initialize keyboard.
******************************************************************************/
static void bspInitButtons(void)
{
  DDRD &= ~((1 << PD1) | (1 << PD2) | (1 << PD3)); // PD1, PD2 and PD3 are inputs
  PORTD |= ((1 << PD1) | (1 << PD2) | (1 << PD3)); // Enable pullups

  DDRB |= 0xFF;  // Set all pins from port B as outputs
  DDRD |= ((1 << PD5) | (1 << PD7));  // PD5, PD7 are outputs

  PORTD &= ~((1 << PD5) | (1 << PD7)); // The default state is "zero"
  PORTB = 0x00;

  __delay_us(20); // Delay to let signals settle

  // Register row inputs interrupts and enable them
  HAL_RegisterIrq(IRQ_1, IRQ_LOW_LEVEL, keyboardInterruptHandler);
  HAL_RegisterIrq(IRQ_2, IRQ_LOW_LEVEL, keyboardInterruptHandler);
  HAL_RegisterIrq(IRQ_3, IRQ_LOW_LEVEL, keyboardInterruptHandler);
  enableIrqs();

  for (uint8_t i = 0; i < N_BUTTONS; i++)
  {
    buttons[i].pressed = false;
    buttons[i].event = BUTTON_NO_EVENT;
    buttons[i].interval = 0;
    buttons[i].threshold = 0;
  }

  scanTimer.interval = SCAN_TIMER_INTERVAL;
  scanTimer.mode = TIMER_ONE_SHOT_MODE;
  scanTimer.callback = scanTimerFired;
}


/**************************************************************************//**
\brief Uninitialize the keyboard
******************************************************************************/
static void bspUninitButtons(void)
{
  disableIrqs();
  HAL_UnregisterIrq(IRQ_1);
  HAL_UnregisterIrq(IRQ_2);
  HAL_UnregisterIrq(IRQ_3);
  HAL_StopAppTimer(&scanTimer);
}

/**************************************************************************//**
\brief Set threshold value for a button
\param[in] button - button index
\param[in] threshold - value to set
******************************************************************************/
void BSP_SetButtonThreshold(BSP_ButtonId_t button, uint8_t threshold)
{
  buttons[button].threshold = threshold;
}

/**************************************************************************//**
\brief Determine if specified button is currently being pressed
\param[in] button - button index
\returns true - button is pressed
\returns false - button is released
******************************************************************************/
bool BSP_IsButtonPressed(BSP_ButtonId_t button)
{
  return buttons[button].pressed;
}

/**************************************************************************//**
\brief Enable interrupts on all of the rows
******************************************************************************/
static void enableIrqs(void)
{
  HAL_EnableIrq(IRQ_1);
  HAL_EnableIrq(IRQ_2);
  HAL_EnableIrq(IRQ_3);
}

/**************************************************************************//**
\brief Disable interrupts on all of the rows
******************************************************************************/
static void disableIrqs(void)
{
  HAL_DisableIrq(IRQ_1);
  HAL_DisableIrq(IRQ_2);
  HAL_DisableIrq(IRQ_3);
}

/**************************************************************************//**
\brief Determine if a button in the column is pressed
\returns true - a button is pressed
\returns false - a button is not pressed
******************************************************************************/
static bool isKeyPressed(uint8_t column, uint8_t row)
{
  bool keyPressed = false;

  STORE_BUTTONS_REGS
  PORTB = 0xFF;
  PORTD |= (1 << PD5) | (1 << PD7);

  if (column < 7)
    PORTB = ~(1 << column) & 0xFF;
  else if (7 == column)
    PORTD = ~(1 << PD5) & 0xFF;
  else if (8 == column)
    PORTD = ~(1 << PD7) & 0xFF;

  asm("nop");
  keyPressed = (PIND & (1 << (row + 1))) == 0;
  RECOVER_BUTTONS_REGS

  return keyPressed;
}

/**************************************************************************//**
\brief Scan the keyboard for events
\returns true - one or more buttons are pressed
\returns false - no buttons are pressed
******************************************************************************/
static bool scanKeyboard(void)
{
  uint8_t row, column;
  uint8_t button = 0;
  bool    ret = false;
  bool    pressed;

  for (row = 0; row < N_ROWS; row++)
  {
    for (column = 0; column < N_COLUMNS; column++)
    {
      pressed = isKeyPressed(column, row);
      if (pressed)
        ret = true;

      if (buttons[button].pressed && pressed)
      {
        if (buttons[button].interval < 255)
        {
          buttons[button].interval++;

          if (buttons[button].interval == buttons[button].threshold)
          {
            buttons[button].event = BUTTON_THRESHOLD_REACHED_EVENT;
            bspPostTask(BSP_BUTTONS);
          }
        }
      }

      else if (buttons[button].pressed != pressed)
      {
        buttons[button].pressed = pressed;
        buttons[button].event = BUTTON_CHANGED_EVENT;
        bspPostTask(BSP_BUTTONS);
      }

      button++;
    }
  }

  return ret;
}

/**************************************************************************//**
\brief Start scanning timer
******************************************************************************/
static void startScanTimer(void)
{
  HAL_StopAppTimer(&scanTimer);
  HAL_StartAppTimer(&scanTimer);
}

/**************************************************************************//**
\brief Scanning timer event
******************************************************************************/
static void scanTimerFired(void)
{
  if (scanKeyboard())
    startScanTimer();
  else
    enableIrqs();
}

/**************************************************************************//**
\brief Handler for the keyboard interrupt
******************************************************************************/
static void keyboardInterruptHandler(void)
{
  disableIrqs();
  interrupt = true;
  bspPostTask(BSP_BUTTONS);
}

/**************************************************************************//**
\brief BSP buttons handler
******************************************************************************/
void bspButtonsHandler(void)
{
  if (interrupt)
  {
    interrupt = false;

    if (scanKeyboard())
      startScanTimer();
    else
      enableIrqs();
  }

  for (uint8_t i = 0; i < N_BUTTONS; i++)
  {
    if (BUTTON_CHANGED_EVENT == buttons[i].event)
    {
      if (buttons[i].pressed)
        buttons[i].event = BUTTON_PRESSED_EVENT;
      else
        buttons[i].event = BUTTON_RELEASED_EVENT;
    }

    if (BUTTON_NO_EVENT != buttons[i].event)
    {
      sysAssert(bspButtonEventHandler, BSPBUTTONS_NULLCALLBACKS_0);
      bspButtonEventHandler(buttons[i].event, i, buttons[i].interval);
    }

    if (BUTTON_RELEASED_EVENT == buttons[i].event)
      buttons[i].interval = 0;

    buttons[i].event = BUTTON_NO_EVENT;
  }
}
#endif // BSP_SUPPORT != BOARD_RCB_KEY_REMOTE

#endif // BSP_SUPPORT != BOARD_FAKE

// end of buttons.c
