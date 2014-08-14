/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: S_Nv_Platform_Ids.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef S_NV_PLATFORM_IDS_H
#define S_NV_PLATFORM_IDS_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "S_Nv.h"

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/**
    This file defines the flash storage ids used by the platform.
    Do not change existing items without careful consideration!
*/
#define FREE_RANGES_STORAGE_ID                      (S_NV_PLATFORM_RANGE_MIN + 1u) // addressManager
#define FACTORY_NEW_STORAGE_ID                      (S_NV_PLATFORM_RANGE_MIN + 2u) // device info
#define FRAMECOUNTER_STORAGE_ID                     (S_NV_PLATFORM_RANGE_MIN + 4u) // NWK security frame counter, persistent
#define BINDING_TABLE_STORAGE_ID                    (S_NV_PLATFORM_RANGE_MIN + 5u) // to be done
#define APS_TRUSTCENTER_ADDRESS_STORAGE_ID          (S_NV_PLATFORM_RANGE_MIN + 6u) // to be done
#define APS_TRUSTCENTER_KEY_STORAGE_ID              (S_NV_PLATFORM_RANGE_MIN + 7u) // to be done

/* Over The Air Upgrade ids */
#define OTA_UPGRADE_STORAGE_ID                      (S_NV_PLATFORM_RANGE_MIN + 10u)

#endif //S_NV_PLATFORM_IDS_H
