/**************************************************************************//**
  \file  halFlash.c

  \brief Implementation of flash access hardware-dependent module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
   27/06/12 D. Kolmakov - Created
  Last change:
    $Id: halFlash.c 25824 2013-12-10 08:44:12Z unithra.c $
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_FLASH_ACCESS)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <halAssert.h>
#include <halFlash.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Forms boot flash request and calls handler in boot section.
  \param[in] flashAccessReq - flash access request
******************************************************************************/
void halFlashAccessReq(HAL_FlashAccessReq_t *flashAccessReq)
{
  HAL_BootSectionAccessPoint_t bootSectionAccessPoint = NULL;
  uint8_t gpior0 = GPIOR0;
  uint8_t gpior1 = GPIOR1;
  
#if defined(__ICCAVR__)
  memcpy_P(&bootSectionAccessPoint, (__farflash void *)BOOT_SECTION_ACCESS_POINT, sizeof(HAL_BootSectionAccessPoint_t));
#elif defined(__GNUC__)
  bootSectionAccessPoint = (void *)pgm_read_word_far(BOOT_SECTION_ACCESS_POINT);
#endif
  GPIOR1 = ((uint16_t)flashAccessReq >> 8);
  GPIOR0 = (uint16_t)flashAccessReq;
  // Call boot section access point
  bootSectionAccessPoint();
  
  // Call the end of the flash access event handler
  HAL_FlashAccessFinishedHandler();
  
  GPIOR0 = gpior0;
  GPIOR1 = gpior1;
}

#endif  // defined(HAL_USE_BOOT_FLASH)
