/************************************************************************//**
  \file appConst.h

  \brief
    Devices related definition.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.12.09 A. Kostyukov - Created.
******************************************************************************/

#ifndef _APPCONSTS_H
#define _APPCONSTS_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <appFramework.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define APP_PROFILE_ID PROFILE_ID_HOME_AUTOMATION

#define APP_ENDPOINT_DIMMABLE_LIGHT          0x11
#define APP_ENDPOINT_DIMMER_SWITCH           0x12
#define APP_ENDPOINT_OCCUPANCY_SENSOR        0x13
#define APP_ENDPOINT_COMBINED_INTERFACE      0x14

// Trust Center device unique extended address.

#if !defined(APP_DEVICE_TYPE_COMBINED_INTERFACE)
  #define TC_EXT_ADDRESS         APS_UNIVERSAL_EXTENDED_ADDRESS
#else
  #define TC_EXT_ADDRESS         APP_COMBINED_INTERFACE_EXT_ADDRESS
#endif // APP_DEVICE_TYPE_DIMMER_SWITCH_TC

// Organize device-related definitions and check device type setting.
// Usually this section shall not be changed by user.
#if defined(APP_DEVICE_TYPE_DIMMABLE_LIGHT)
  #define APP_EXT_ADDRESS APP_DIMMABLE_LIGHT_EXT_ADDRESS
  #define APP_DEVICE_TYPE DEV_TYPE_ROUTER
  #define APP_SRC_ENDPOINT_ID APP_ENDPOINT_DIMMABLE_LIGHT

#elif defined(APP_DEVICE_TYPE_DIMMER_SWITCH)
  #define APP_EXT_ADDRESS APP_DIMMER_SWITCH_EXT_ADDRESS
  #define APP_DEVICE_TYPE DEV_TYPE_ENDDEVICE
  #define APP_SRC_ENDPOINT_ID APP_ENDPOINT_DIMMER_SWITCH

#elif defined(APP_DEVICE_TYPE_OCCUPANCY_SENSOR)
  #define APP_EXT_ADDRESS APP_OCCUPANCY_SENSOR_EXT_ADDRESS
  #define APP_DEVICE_TYPE DEV_TYPE_ENDDEVICE
  #define APP_SRC_ENDPOINT_ID APP_ENDPOINT_OCCUPANCY_SENSOR

#elif defined (APP_DEVICE_TYPE_COMBINED_INTERFACE)
  #define APP_EXT_ADDRESS APP_COMBINED_INTERFACE_EXT_ADDRESS
  #define APP_DEVICE_TYPE DEV_TYPE_COORDINATOR
  #define APP_SRC_ENDPOINT_ID APP_ENDPOINT_COMBINED_INTERFACE
#else
  #error Device type is not detected
#endif

#define CS_UID                              APP_EXT_ADDRESS
#define CS_APS_TRUST_CENTER_ADDRESS         TC_EXT_ADDRESS

#ifndef APP_NETWORK_KEY
  #define APP_NETWORK_KEY {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}
#endif //#ifndef APP_NETWORK_KEY


#define HA_LINK_KEY {0x5a,0x69,0x67,0x42,0x65,0x65,0x41,0x6c,0x6c,0x69,0x61,0x6e,0x63,0x65,0x30,0x39}
// For ZLL networks compatibility
//#define HA_LINK_KEY {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xdA, 0xdb, 0xdc, 0xdd, 0xde, 0xdf}

#define HA_APP_MEMORY_MEM_ID         APP_DIR1_MEM_ID
#define APP_DL_SCENES_MEM_ID         APP_PARAM1_MEM_ID
#define APP_DL_ONOFF_MEM_ID          APP_PARAM2_MEM_ID
#define APP_DL_LEVEL_CONTROL_MEM_ID  APP_PARAM3_MEM_ID
#define APP_OS_OCCUPANCY_MEM_ID      APP_PARAM1_MEM_ID

#endif // _APPCONSTS_H

// eof appConsts.h

