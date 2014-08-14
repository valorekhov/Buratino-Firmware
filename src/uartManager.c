
#if (APP_DEVICE_EVENTS_LOGGING == 1) || (APP_ENABLE_CONSOLE == 1)

/*******************************************************************************
                             Includes section
*******************************************************************************/
#include <uartManager.h>
#include <console.h>
#include <stdarg.h>
#include <stdio.h>
#ifdef BSP_ENABLE_RS232_CONTROL
#include <rs232Controller.h>
#endif /* BSP_ENABLE_RS232_CONTROL */

/*******************************************************************************
                             Definition(s) section
*******************************************************************************/
/* This should be large enough to accommodate the longest output console string. */
#define TMP_STRING_BUFFER_SIZE              114
// UART Tx buffer size
#ifndef APP_UART_TX_BUFFER_SIZE
  #define APP_UART_TX_BUFFER_SIZE           510 // Size to accommodate complete menu,
#endif                                          // can be increased/decreased based on the requirement

/*******************************************************************************
                             Local variables section
*******************************************************************************/
static HAL_UsartDescriptor_t appUsartDescriptor;      // UART descriptor (required by stack)
static uint8_t uartTxBuffer[APP_UART_TX_BUFFER_SIZE]; // UART Tx buffer
static uint8_t uartRxBuffer[USART_RX_BUFFER_LENGTH];  // UART Rx buffer

/*******************************************************************************
                             Prototypes section
*******************************************************************************/
static void uartDataReceived(uint16_t length);

/*******************************************************************************
                             Implementation section
*******************************************************************************/
/**************************************************************************//**
/brief Inits UART, register UART callbacks
*******************************************************************************/
void uartInit(void)
{
  appUsartDescriptor.tty             = USART_CHANNEL;
  appUsartDescriptor.mode            = USART_MODE_ASYNC;
  appUsartDescriptor.baudrate        = USART_BAUDRATE_38400;
  appUsartDescriptor.dataLength      = USART_DATA8;
  appUsartDescriptor.parity          = USART_PARITY_NONE;
  appUsartDescriptor.stopbits        = USART_STOPBIT_1;
  appUsartDescriptor.rxBuffer        = uartRxBuffer;
  appUsartDescriptor.rxBufferLength  = USART_RX_BUFFER_LENGTH;
  appUsartDescriptor.txBuffer        = uartTxBuffer;
  appUsartDescriptor.txBufferLength  = APP_UART_TX_BUFFER_SIZE;
  appUsartDescriptor.rxCallback      = uartDataReceived;
  appUsartDescriptor.txCallback      = NULL;
  appUsartDescriptor.flowControl     = USART_FLOW_CONTROL_NONE;

#if BSP_ENABLE_RS232_CONTROL == 1 && !defined(OTAU_SERVER)
  BSP_EnableRs232();
#endif
  OPEN_USART(&appUsartDescriptor);
}

/**************************************************************************//**
\brief Sends data to the Serial Interface

\param[in] data   - pointer to data to be sent;
\param[in] length - number of bytes (length) of data to be sent
******************************************************************************/
void sendDataToUart(uint8_t *data, uint8_t length)
{
  WRITE_USART(&appUsartDescriptor, data, length);
}

/**************************************************************************//**
\brief Reads data from the Serial Interface

\params[out] data - pointer to receive buffer;
\params[in]  length - number of bytes (length) in receive buffer;

\returns amount of read bytes
******************************************************************************/
uint8_t readDataFromUart(uint8_t *data, uint8_t length)
{
  return READ_USART(&appUsartDescriptor, data, length);
}

/**************************************************************************//**
\brief Callback on receiving data via Serial interface

\param[in] length - amount of received bytes
*******************************************************************************/
static void uartDataReceived(uint16_t length)
{
#if APP_ENABLE_CONSOLE == 1
  processConsole(length);
#endif // APP_ENABLE_CONSOLE == 1
}

#if APP_DEVICE_EVENTS_LOGGING == 1 && !defined BOARD_PC
#if defined(__GNUC__) && (defined(AT91SAM7X256) || defined(AT91SAM3S4C))
uint8_t* _sbrk(int incr)
{
  static uint8_t heap[100];
  static uint8_t *heap_end;
  uint8_t *prev_heap_end;

  if (0 == heap_end)
    heap_end = heap;

  prev_heap_end = heap_end;

  if ((heap_end + incr) >= (heap + sizeof(heap)))
    return NULL;

  heap_end += incr;
  return prev_heap_end;
}
#endif /* __GNUC__ */

/**************************************************************************//**
\brief Outputs formated string to serial interface

\param[in] formt - formatted string pointer
******************************************************************************/
void appSnprintf(const char *formt, ...)
{
  uint8_t str[TMP_STRING_BUFFER_SIZE];
  uint8_t length;
  va_list ap;

  va_start(ap, formt);
  length = vsnprintf((char*) str, TMP_STRING_BUFFER_SIZE, formt, ap);
  va_end(ap);
  sendDataToUart(str, length);
}
#endif // APP_DEVICE_EVENTS_LOGGING
#endif // (APP_DEVICE_EVENTS_LOGGING == 1) || (APP_ENABLE_CONSOLE == 1)

// eof uartManager.c
