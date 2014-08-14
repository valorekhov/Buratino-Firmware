/************************************************************************//**
  \file wlPdsMemIds.h

  \brief
    PDS file and directory memory identificators definitions

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.05.13 N. Fomin - Created.
******************************************************************************/

#ifndef _WLPDSMEMIDS_H_
#define _WLPDSMEMIDS_H_

/******************************************************************************
                               Includes section
******************************************************************************/
#include <sysTypes.h>

/* Compatibility with std PDS mem IDs  */

#define BC_EXT_GEN_MEMORY_MEM_ID              BC_EXT_GEN_MEMORY_ITEM_ID
#define CS_NEIB_TABLE_MEM_ID                  CS_NEIB_TABLE_ITEM_ID
#define CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID    CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID
#define CS_NWK_SECURITY_IB_MEM_ID             CS_NWK_SECURITY_IB_ITEM_ID
#define CS_NWK_SECURITY_KEYS_MEM_ID           CS_NWK_SECURITY_KEYS_ITEM_ID
#define NWK_SECURITY_COUNTERS_MEM_ID          NWK_SECURITY_COUNTERS_ITEM_ID
#define CS_APS_BINDING_TABLE_MEM_ID           CS_APS_BINDING_TABLE_ITEM_ID
#define CS_GROUP_TABLE_MEM_ID                 CS_GROUP_TABLE_ITEM_ID
#define NWK_RREQ_IDENTIFIER_MEM_ID            NWK_RREQ_IDENTIFIER_ITEM_ID

#define BC_EXT_GEN_MEMORY_ITEM_ID           0x0001U
#define CS_NEIB_TABLE_ITEM_ID               0x0002U
#define CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID 0x0003U
#define CS_NWK_SECURITY_IB_ITEM_ID          0x0004U
#define CS_NWK_SECURITY_KEYS_ITEM_ID        0x0005U
#define NWK_SECURITY_COUNTERS_ITEM_ID       0x0006U
#define CS_APS_BINDING_TABLE_ITEM_ID        0x0007U
#define CS_GROUP_TABLE_ITEM_ID              0x0008U
#define NWK_RREQ_IDENTIFIER_ITEM_ID         0x0009U
#define BITCLOUD_MAX_ITEMS_AMOUNT           NWK_RREQ_IDENTIFIER_ITEM_ID

/* Application files IDs */
#define  APP_PARAM1_MEM_ID                  0x000AU
#define  APP_PARAM2_MEM_ID                  0x000BU
#define  APP_PARAM3_MEM_ID                  0x000CU
#define  APP_PARAM4_MEM_ID                  0x000DU
#define  APP_PARAM5_MEM_ID                  0x000EU
#define  APP_PARAM6_MEM_ID                  0x000FU
#define  APP_PARAM7_MEM_ID                  0x0010U

/* Directory mask */
#define  PDS_DIRECTORY_MASK                 0x4000U

#define BC_ALL_MEMORY_MEM_ID                0x4001U
#define BITCLOUD_MAX_DIRECTORIES_AMOUNT     (1)

#define PDS_ALL_EXISTENT_MEMORY             0x4002U

/* Application directory IDs */
#define  APP_DIR1_MEM_ID                    0x4003U
#define  APP_DIR2_MEM_ID                    0x4004U
#define  APP_DIR3_MEM_ID                    0x4005U
#define  APP_DIR4_MEM_ID                    0x4006U

/* Application files and directories amount */
#ifndef APPLICATION_MAX_FILES_AMOUNT
  #define APPLICATION_MAX_FILES_AMOUNT      0x00010U
#endif /* APPLICATION_MAX_FILES_AMOUNT */

#ifndef APPLICATION_MAX_DIRECTORIES_AMOUNT
  #define APPLICATION_MAX_DIRECTORIES_AMOUNT 0x0004U
#endif /* APPLICATION_MAX_DIRECTORIES_AMOUNT */

#define PDS_ITEM_AMOUNT                     (BITCLOUD_MAX_ITEMS_AMOUNT + APPLICATION_MAX_FILES_AMOUNT)

#define PDS_DIRECTORIES_AMOUNT              (BITCLOUD_MAX_DIRECTORIES_AMOUNT + APPLICATION_MAX_DIRECTORIES_AMOUNT)

/* Total files and directories amount supported by PDS */
#define PDS_ITEM_IDS_AMOUNT                 (PDS_ITEM_AMOUNT +  PDS_DIRECTORIES_AMOUNT)

#define PDS_ITEM_MASK_SIZE                  (PDS_ITEM_AMOUNT / 8U + (PDS_ITEM_AMOUNT % 8U ? 1U : 0U))

/******************************************************************************
                               Types section
******************************************************************************/

/*! Type of PDS file or directory unique identifier. */
typedef uint16_t PDS_MemId_t;

/* Memory identifier record pointer */
typedef const PDS_MemId_t FLASH_PTR *PDS_MemIdRec_t;

#endif // _WLPDSMEMIDS_H_
/* eof wlPdsMemIds.h */