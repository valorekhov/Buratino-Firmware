/**************************************************************************//**
\file  ofdSerializer.c

\brief Implementation of capturing of serial interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    7/08/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if (APP_USE_OTAU == 1)
#if APP_USE_FAKE_OFD_DRIVER == 0

/******************************************************************************
                   Includes section
******************************************************************************/
#include <ofdExtMemory.h>
#include <ofdMemoryDriver.h>
#include <spi.h>
#include <driversDbg.h>

/******************************************************************************
                   External variables section
******************************************************************************/
extern OFD_Callback_t ofdCallback;

/******************************************************************************
                   Global variables section
******************************************************************************/
HAL_SpiDescriptor_t spiDesc;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Opens serial interface and checks memory type.

\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_Open(OFD_Callback_t cb)
{
  ofdCallback = cb;

#if defined(ATMEGA1281) || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  spiDesc.tty       = SPI_CHANNEL_0;
  spiDesc.baudRate  = SPI_CLOCK_RATE_2000;
#elif  defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
  spiDesc.tty       = SPI_CHANNEL_D;
  spiDesc.baudRate  = SPI_CLOCK_RATE_8000;
#endif
  spiDesc.clockMode = SPI_CLOCK_MODE3;
  spiDesc.dataOrder = SPI_DATA_MSB_FIRST;
  spiDesc.callback  = NULL;

  if (-1 == HAL_OpenSpi(&spiDesc))
  {
    sysAssert(ofdCallback, OFD_NULLCALLBACK0);
    ofdCallback(OFD_SERIAL_INTERFACE_BUSY);
  }

  GPIO_EXT_MEM_CS_set();
  GPIO_EXT_MEM_CS_make_out();
  ofdFindStorageSpace();
#if EXTERNAL_MEMORY == AT25DF041A
  ofdUnprotectMemorySectors();
#endif
}

/**************************************************************************//**
\brief Closes serial interface.
******************************************************************************/
void OFD_Close(void)
{
  HAL_CloseSpi(&spiDesc);
}

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1

// eof ofdSerialize.c
