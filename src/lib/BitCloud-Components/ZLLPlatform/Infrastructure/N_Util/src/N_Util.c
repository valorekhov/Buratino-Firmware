/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Util.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Util_Bindings.h"

#include "N_Util.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Types.h"
#include "S_SerialComm.h"

#if defined(__ICCAVR__)
#  include <intrinsics.h>
#endif

#if defined(__COVERITY__)
#  include <stdlib.h>
#endif

#include <resetReason.h>

#if (defined(__ATMEL_BITCLOUD__))
#include <sysUtils.h> // for SYS_GetRandomNumber
#endif

//__near_func void __program_start(void);

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Util"


/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

uint8_t N_Util_HighestNetworkUpdateId(uint8_t id1, uint8_t id2)
{
    if (id1 > id2)
    {
        if ( (uint8_t) (id1 - id2) > 200u )
        {
            return id2;
        }
        else
        {
            return id1;
        }
    }
    else
    {
        if ( (uint8_t) (id2 - id1) > 200u )
        {
            return id1;
        }
        else
        {
            return id2;
        }
    }
}

#if defined(__ICCAVR__) &&  (defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2))

void N_Util_SystemReset(void)
{
    __watchdog_reset();

    MCUSR = 0;
    MCUSR &= ~(1 << WDRF);
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIF) | (0 << WDIE) | (1 << WDCE) | (1 << WDE);

    // loop until reset
    for (;;)
    {
    }
}

#elif defined (__WIN32__)

void N_Util_SystemReset(void)
{
    // Simulate a reset
    S_SerialComm_TxMessage("System", "Ready", 0);
}

#else 

void N_Util_SystemReset(void)
{
  HAL_WarmReset();
}

#endif

#if defined(__ATMEL_BITCLOUD__)

uint16_t N_Util_Random_Impl(void)
{
    return SYS_GetRandomNumber();
}

#endif
