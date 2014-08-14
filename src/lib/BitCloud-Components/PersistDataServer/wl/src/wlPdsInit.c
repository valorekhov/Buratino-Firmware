/**************************************************************************//**
  \file pdsInit.c

  \brief Persistent Data Server initialization module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    21.05.13 N. Fomin - Created
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                   Includes section
******************************************************************************/
#include <wlPdsBindings.h>
#include <S_Nv_Init.h>
#include <S_Nv.h>
#include <N_ErrH.h>
#include <D_Nv_Init.h>
#include <wlPdsTypesConverter.h>
#include <wlPdsMemIds.h>
#include <flash.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define COMPID "pwlPdsInit"

#if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
#ifdef PDS_NO_BOOTLOADER_SUPPORT
DECLARE_FLASH_ACCESS_ENTRY_POINT();
#endif /* PDS_NO_BOOTLOADER_SUPPORT */
#endif /* defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2) */

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
\brief Persistent Data Server initialization routine.
******************************************************************************/
void PDS_Init(void)
{
  D_Nv_Init();
  S_Nv_Init();
}

/******************************************************************************
\brief PDS item initialization routine
******************************************************************************/
void PDS_InitItems(void)
{
  S_Nv_ReturnValue_t ret;
  ItemIdToMemoryMapping_t itemDesc;

  for (S_Nv_ItemId_t i = 1U; i < BITCLOUD_MAX_ITEMS_AMOUNT + 1; i++)
  {
    if (pdsGetItemDescr(i, &itemDesc))
    {
      N_ERRH_ASSERT_FATAL(itemDesc.itemSize);

      ret = S_Nv_ItemInit(i, itemDesc.itemSize, NULL);
      N_ERRH_ASSERT_FATAL((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));
    }
  }
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING == 1 */
/* eof pdsInit.c */

