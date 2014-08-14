/******************************************************************************
  \file csMem.c

  \brief
    BitCloud information bases memory

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.10.10 A. Razinkov - Created.
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <csBuffers.h>
#include <macenvMem.h>
#ifndef _MAC2_
#include <zdoZib.h>
#endif
#include <csSIB.h>
#include <configServer.h>

/******************************************************************************
                    External variables section
******************************************************************************/
#if defined(_USE_KF_MAC_)
extern uint64_t tal_pib_IeeeAddress;
#endif // defined(_USE_KF_MAC_)

/******************************************************************************
                    Global variables section
******************************************************************************/
/* BitCloud information bases allocation */
#if !defined(_USE_KF_MAC_)
PIB_t csPIB;
#endif // defined(_USE_KF_MAC_)

#ifndef _MAC2_
NIB_t csNIB;
ZIB_t csZIB;
AIB_t csAIB;
#endif
SIB_t csSIB;

/* BitCloud memory buffers allocation */
CS_StackBuffers_t stackBuffers;

/* FLASH memory allocation to store read-only parameters */

/* Default information bases item values */
#ifndef _MAC2_
NIB_t PROGMEM_DECLARE(defaultNIB) =
{
#if defined _SECURITY_
  .securityIB = {
    .securityLevel = CS_SECURITY_LEVEL,
    .secureAllFrames = CS_SECURITY_ALL_FRAMES
  },
#endif
  .deviceType = CS_DEVICE_TYPE,
  .addrAlloc = CS_ADDRESS_ASSIGNMENT_METHOD,
  .symLink = true,
  .uniqueAddr = CS_NWK_UNIQUE_ADDR,
  .leaveReqAllowed = CS_NWK_LEAVE_REQ_ALLOWED,
  .stackProfile = CS_STACK_PROFILE,
  .protocolVersion = CS_PROTOCOL_VERSION,
  .maxDepth = CS_MAX_NETWORK_DEPTH,
  .maxNeighborRouteCost = CS_MAX_NEIGHBOR_ROUTE_COST,
  .maxLinkRouteCost = CS_MAX_LINK_ROUTE_COST,
#if defined _NWK_THRESHOLD_ROUTING_
  .softLinkCostThreshold = CS_SOFT_LINK_COST_THRESHOLD,
#endif /* _NWK_THRESHOLD_ROUTING_ */
  .channelPage = CS_CHANNEL_PAGE,
  .logicalChannel = 0,
  .networkAddress = CCPU_TO_LE16(CS_NWK_ADDR),
  .panId = CCPU_TO_LE16(CS_NWK_PANID),
  .extendedPanId = CCPU_TO_LE64(CS_NWK_EXT_PANID), /* uses as joined-to-network flag */
  .maxRouters = CS_MAX_CHILDREN_ROUTER_AMOUNT,
  .maxEndDevices = CS_MAX_CHILDREN_AMOUNT - CS_MAX_CHILDREN_ROUTER_AMOUNT
};

ZIB_t PROGMEM_DECLARE(defaultZIB) =
{
  .permitJoinDuration = CS_PERMIT_DURATION,
  .channelMask = CS_CHANNEL_MASK,
  .zdpResponseTimeout = CS_ZDP_RESPONSE_TIMEOUT,
  .scanDuration = CS_SCAN_DURATION,
};

AIB_t PROGMEM_DECLARE(defaultAIB) =
{
  .nonMemberRadius = APS_AIB_NONMEMBER_RADIUS_DEFAULT_VALUE, /* See ZigBee spec r19, Table 2.24. */
  .maxFrameRetries = CS_APS_MAX_FRAME_RETRIES,
#ifdef _APS_FRAGMENTATION_
  .fragWindowSize = CS_APS_MAX_TRANSMISSION_WINDOW_SIZE,
#endif /* _APS_FRAGMENTATION_ */
#ifdef _SECURITY_
  .trustCenterAddress = CCPU_TO_LE64(CS_APS_TRUST_CENTER_ADDRESS),
  .tcNwkAddr = CCPU_TO_LE16(CS_SHORT_TRUST_CENTER_ADDRESS),
#ifdef _LINK_SECURITY_
  .securityPolicy = {0x00, 0x07, 0x07, 0x07,
#ifdef _LIGHT_LINK_PROFILE_
                     0x07, 0x7F, 0x1a, 0x78,
#else
                     0x07, 0x7F, 0x7a, 0x78,
#endif // _LIGHT_LINK_PROFILE_
                     0x5a, 0x5f, 0x07, 0x07,
                     0x07, 0x07, 0x07, 0x78,
                     0x7F, 0x1F}
#endif /* _LINK_SECURITY_ */
#endif /* _SECURITY_ */
};
#endif /* !_MAC2_*/

SIB_t PROGMEM_DECLARE(defaultSIB) =
{
  /* MAC parameters */
  .csMacTransactionTime = CS_MAC_TRANSACTION_TIME,
  .csRfTxPower = CS_RF_TX_POWER,
#ifdef AT86RF212
  .csLbtMode = CS_LBT_MODE,
#endif //AT86RF212
#ifdef _FFD_
#ifdef _PENDING_EMPTY_DATA_FRAME_
  .csMacSendEmptyDataFrame = CS_MAC_SEND_EMPTY_DATA_FRAME,
#endif /*_PENDING_EMPTY_DATA_FRAME_*/
#endif /*_FFD_*/
#ifndef _MAC2_

  /* NWK parameters */
  .csNwkPanid = CCPU_TO_LE16(CS_NWK_PANID),
  .csNwkUseMulticast = CS_NWK_USE_MULTICAST,
  .csNwkMaxLinkStatusFailures = CS_NWK_MAX_LINK_STATUS_FAILURES,
  .csNwkEndDeviceMaxFailures = CS_NWK_END_DEVICE_MAX_FAILURES,
  .csNwkLogicalChannel = 0,
  .csNwkPredefinedPanid = CS_NWK_PREDEFINED_PANID,
  .csAddressAssignmentMethod = CS_ADDRESS_ASSIGNMENT_METHOD,
#ifdef _NWK_CONCENTRATOR_
  .csNwkConcentratorDiscoveryTime = NWK_CONCENTRATOR_DISCOVERY_TIME,
#endif
  .csJoinCtrl = {.method = NWK_JOIN_BY_DEFAULT},
  .csLeaveRejoinCtrl = NWK_DEFINE_LEAVE_CONTROL_DEFAULT,
  .csLeaveNonRejoinCtrl = NWK_DEFINE_LEAVE_CONTROL_DEFAULT,
  /* ZDO parameters */
  .csEndDeviceSleepPeriod = CS_END_DEVICE_SLEEP_PERIOD,
  .csFfdSleepPeriod = CS_FFD_SLEEP_PERIOD,
  .csRxOnWhenIdle = (CS_DEVICE_TYPE == DEVICE_TYPE_END_DEVICE) ? CS_RX_ON_WHEN_IDLE : true,
  .csComplexDescriptorAvailable = CS_COMPLEX_DESCRIPTOR_AVAILABLE,
  .csUserDescriptorAvailable = CS_USER_DESCRIPTOR_AVAILABLE,
  .csUserDescriptor = {.FieldLength = ZDP_USER_DESC_FIELD_SIZE, .FieldName = "Atmel           "},
  .csIndirectPollRate = CS_INDIRECT_POLL_RATE,
  .csZdoJoinAttempts = CS_ZDO_JOIN_ATTEMPTS,
  .csZdoJoinInterval = CS_ZDO_JOIN_INTERVAL,
  .csExtPANID = CCPU_TO_LE64(CS_EXT_PANID), /* user-defined PAN ID */

  /* PDS parameters*/
  .csDtrWakeup = CS_DTR_WAKEUP,

  /* ZCL parameters */
#if ZCL_SUPPORT == 1
  #if APP_USE_OTAU == 1
  .csOtauDefaultServerAddress = CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS,
  .csOtauServerDiscoveryPeriod = CS_ZCL_OTAU_SERVER_DISCOVERY_PERIOD,
  .csOtauServerPageRequestUsage = CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE,
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */

    /* Security parameters */
#ifdef _SECURITY_
  .csSecurityOn = CS_SECURITY_ON,
  .csZdoSecurityStatus = CS_ZDO_SECURITY_STATUS,
#endif /* _SECURITY_ */
#endif /* _MAC2_ */
};

#ifdef _SECURITY_
uint8_t defaultKey[SECURITY_KEY_SIZE] = CS_NETWORK_KEY;
#endif /* _SECURITY_ */

CS_ReadOnlyItems_t PROGMEM_DECLARE(csReadOnlyItems) =
{
  /* MAC parameters */
  .csMacFrameRxBufferSize = CS_MAC_FRAME_RX_BUFFER_SIZE,
#ifdef _FFD_
  .csMacFrameDataReqBufferSize = CS_MAC_FRAME_DATA_REQ_BUFFER_SIZE,
#endif
#ifdef _MAC_BAN_NODE_
  .csMacBanTableSize = CS_BAN_TABLE_SIZE,
#endif /* _MAC_BAN_NODE_ */

  /* NWK parameters */
#ifndef _MAC2_
  .csNwkRouteCacheSize = CS_ROUTE_CACHE_SIZE,
  .csNeibTableSize = CS_NEIB_TABLE_SIZE,
  .csMacPanDescriptorAmount = CS_NEIB_TABLE_SIZE,
  .csMaxChildrenAmount = CS_MAX_CHILDREN_AMOUNT,
  .csMaxChildrenRouterAmount = CS_MAX_CHILDREN_ROUTER_AMOUNT,
  .csMaxNetworkDepth = CS_MAX_NETWORK_DEPTH,
  .csNwkBuffersAmount = CS_NWK_BUFFERS_AMOUNT,
  .csJoinIndObjAmount = CS_JOIN_IND_OBJ_AMOUNT,
  .csRouteTableSize = CS_ROUTE_TABLE_SIZE,
  .csAddressMapTableSize = CS_ADDRESS_MAP_TABLE_SIZE,
  .csRouteDiscoveryTableSize = CS_ROUTE_DISCOVERY_TABLE_SIZE,
  .csNwkBttSize = CS_NWK_BTT_SIZE,
  .csNwkMaxPermitJoinPeriodLogic = CS_NWK_MAX_PERMIT_JOIN_PERIOD_LOGIC,
#ifdef _GROUP_TABLE_
  .csGroupTableSize = CS_GROUP_TABLE_SIZE,
#endif /* _GROUP_TABLE_ */
#ifdef _NWK_PASSIVE_ACK_
  .csPassiveAckAmount = CS_NWK_PASSIVE_ACK_AMOUNT,
#endif /* _NWK_PASSIVE_ACK_ */

  /* ZDO parameters */
  .csZdoMaxSyncFailAmount = CS_ZDO_MAX_SYNC_FAIL_AMOUNT,
  .csZdoParentLinkRetryTreshold = CS_ZDO_PARENT_LINK_RETRY_THRESHOLD,

  /* APS parameters */
  .csApsDataReqBuffersAmount = CS_APS_DATA_REQ_BUFFERS_AMOUNT,
  .csApsAckFrameBuffesAmount = CS_APS_ACK_FRAME_BUFFERS_AMOUNT,
  .csDuplicateRejectionTableSize = CS_DUPLICATE_REJECTION_TABLE_SIZE,
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  .csApsDuplicateEntryCounterSize = CS_APS_DUPLICATE_ENTRY_COUNTER_SIZE,
#endif /* _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_ */
  .csApsDuplicateEntryLifeTime = CS_APS_DUPLICATE_ENTRY_LIFE_TIME,
#ifdef _BINDING_
  .csApsBindingTableSize = CS_APS_BINDING_TABLE_SIZE,
#endif /* _BINDING_ */
#ifdef _APS_FRAGMENTATION_
  .csApsMaxBlocksAmount = CS_APS_MAX_BLOCKS_AMOUNT,
  .csApsBlockSize = CS_APS_BLOCK_SIZE,
  .csApsTxBlockSize = CS_APS_TX_BLOCK_SIZE,
  .csApsRxBlockSize = CS_APS_RX_BLOCK_SIZE,
#endif /* _APS_FRAGMENTATION_ */

#if ZCL_SUPPORT == 1
  .csZclMemoryBuffersAmount = CS_ZCL_MEMORY_BUFFERS_AMOUNT,
  .csZclMemoryBuffersSize = CS_ZCL_BUFFER_SIZE,
  #if APP_USE_OTAU == 1
    .csOtauDiscoveredServerAmount = CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT,
     // server needs two buffers for incoming packets for each client session
    .csOtauClientSessionAmount = CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT * 2,
    #if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      .csOtauServerPageRequestResponseSpacing = CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING,
      .csOtauServerPageSize = CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE,
    #endif /* APP_SUPPORT_OTAU_PAGE_REQUEST == 1 */
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */

#ifdef _SECURITY_
  .csApsSecurityTimeoutPeriod = CS_APS_SECURITY_TIMEOUT_PERIOD,
  .csNwkSecurityKeysAmount = CS_NWK_SECURITY_KEYS_AMOUNT,
#ifdef _TRUST_CENTRE_
  .csMaxTcAuthenticProcessAmount = CS_MAX_TC_AUTHENTIC_PROCESS_AMOUNT,
#endif /* _TRUST_CENTRE_ */
#ifdef _LINK_SECURITY_
  #ifdef _TRUST_CENTRE_
  .csMaxTcEstablishKeyAmount = CS_MAX_TC_ESTABLISH_KEY_AMOUNT,
  #endif /* _TRUST_CENTRE_ */
  .csApsKeyPairDescriptorsAmount = CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT,
#endif /* _LINK_SECURITY_ */
#endif /* _SECURITY_ */
#ifdef _PERMISSIONS_
  .csPermissionsTableSize = CS_PERMISSIONS_TABLE_SIZE,
#endif /* _PERMISSIONS_ */
  /* Other parameters */
  .csStackVersion = CS_STACK_VERSION,
  .csManufacturerCode = CCPU_TO_LE16(CS_MANUFACTURER_CODE),
  .csZdoCommandsAmount = CS_ZDO_COMMANDS_AMOUNT
#endif /* !_MAC2_*/
};

/*
 * \brief Configuration Server items information table stored in FLASH.
 *        Table record contains informaion about corresponding item displacement and size.
 *        Records are automatically sorted by item location and type
 *        (atomic parameter or memory region).
 */
#define RAM_PARAMETER(label, id, addr) [id] = {{(const void FLASH_PTR*)&(addr)}, sizeof(addr)},
#define DUMMY_RAM_PARAMETER(label, id) [id] = {{NULL}, 0},
#define FLASH_PARAMETER(label, id, addr) [id] = {{(const void FLASH_PTR*)&(addr)}, sizeof(addr)},
#define DUMMY_FLASH_PARAMETER(label, id) [id] = {{NULL}, 0},
#define MEMORY_REGION(label, id, addr) [id] = {{(const void FLASH_PTR*)&(addr)}, 0},
#define DUMMY_MEMORY_REGION(label, id) [id] = {{NULL}, 0},

CS_MemoryItem_t PROGMEM_DECLARE(csVarItems[]) =
{
  #include "csVarTable.h"
};

CS_MemoryItem_t PROGMEM_DECLARE(csConstItems[]) =
{
  #include "csConstTable.h"
};

CS_MemoryItem_t PROGMEM_DECLARE(csMemItems[]) =
{
  #include "csMemTable.h"
};

#undef RAM_PARAMETER
#undef DUMMY_RAM_PARAMETER
#undef FLASH_PARAMETER
#undef DUMMY_FLASH_PARAMETER
#undef MEMORY_REGION
#undef DUMMY_MEMORY_REGION

/******************************************************************************
                    Implementation section
******************************************************************************/

/******************************************************************************
\brief Set Configuration Server memory to its default state
******************************************************************************/
void csSetToDefault(void)
{
  /* Restore information bases default values from FLASH */
#if defined(_USE_KF_MAC_)
  tal_pib_IeeeAddress = CCPU_TO_LE64(CS_UID);
  maxFrameTransmissionTime = CS_MAX_FRAME_TRANSMISSION_TIME;
#else
  memset(&csPIB, 0x00, sizeof(PIB_t));
  csPIB.macAttr.extAddr = CCPU_TO_LE64(CS_UID);
  csPIB.macAttr.maxFrameTransmissionTime = CS_MAX_FRAME_TRANSMISSION_TIME;
#endif // defined(_USE_KF_MAC_)

#ifndef _MAC2_
  memcpy_P(&csNIB, &defaultNIB, sizeof(NIB_t));
  memcpy_P(&csZIB, &defaultZIB, sizeof(ZIB_t));
  memcpy_P(&csAIB, &defaultAIB, sizeof(AIB_t));
#endif
  memcpy_P(&csSIB, &defaultSIB, sizeof(SIB_t));
  memset(&stackBuffers, 0x00, sizeof(CS_StackBuffers_t));
}

/* eof csMem.c */

