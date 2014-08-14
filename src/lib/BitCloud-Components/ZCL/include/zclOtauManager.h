/**************************************************************************//**
  \file zclOTAUManager.h

  \brief Declaration of the OTAU manager interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    16.05.11 A. Khromykh - Created.
  Last change:
    $Id: zclOtauManager.h 26289 2014-02-10 14:09:50Z agasthian.s $
*******************************************************************************/
#ifndef _ZCLOTAUMANAGER_H
#define _ZCLOTAUMANAGER_H

/******************************************************************************
                        Defines section
******************************************************************************/
#define OTAU_BLOCK_REQUEST_USAGE         1
#define OTAU_PAGE_REQUEST_USAGE          0
#define OTAU_GET_MISSED_BYTES            1
#define OTAU_NOT_GET_MISSED_BYTES        0
#define OTAU_FIRST_CLIENT_START          1
#define OTAU_CONTINUE_CLIENT_WORK        0
#define OTAU_BLOCK_REQUEST_DELAY_OFF     0
#define OTAU_BLOCK_REQUEST_DELAY_ON      1

/******************************************************************************
                           Types section
******************************************************************************/
typedef struct
{ /* memory for storage of server discovery result */
  struct
  {
    void *next;
  } service;

  bool        busy;
  Endpoint_t  serverEndpoint;
  ShortAddr_t serverShortAddress;
  ExtAddr_t   serverExtAddress;
} ZclOtauDiscoveryResult_t;

typedef struct
{
  struct
  {
    void *next;
  } service;

  bool                           busy;
  uint8_t                        id;
  ZCL_Addressing_t               addressing;
  ZCL_Request_t                  zclCommandReq;

  union
  {
    ZCL_OtauQueryNextImageReq_t  queryNextImageReq;
    ZCL_OtauQueryNextImageResp_t queryNextImageResp;
    ZCL_OtauImageBlockReq_t      imageBlockReq;
    ZCL_OtauImageBlockResp_t     imageBlockResp;
    ZCL_OtauUpgradeEndReq_t      upgradeEndReq;
    ZCL_OtauUpgradeEndResp_t     upgradeEndResp;
  };
  ZCL_OtauImagePageReq_t         imagePageReq;
} ZclOtauServerTransac_t;

typedef struct
{
  OtauReqMemory_t              reqMem;
  OtauZclReqMemory_t           zclReqMem;
  HAL_AppTimer_t               genericTimer;
  HAL_AppTimer_t               pageRequestTimer;
  ZclOtauClientImageBuffer_t   otauParam;
  OtauImageAuxVar_t            imageAuxParam;
  OFD_MemoryAccessParam_t      memParam;
  ZCL_OtauFirmwareVersion_t    newFirmwareVersion;

  uint8_t                      discoveredServerAmount;
  ZclOtauDiscoveryResult_t    *discoveredServerMem;

  uint8_t                     *missedBytesMask;

  bool                         isOtauStopTriggered;
  struct
  {
    uint8_t                    blockRequest : 1;
    uint8_t                    missedBytesGetting : 1;
    uint8_t                    firstStart : 1;
    uint8_t                    blockRequestDelayOn : 1;
  };
} ZCL_OtauClientMem_t;

typedef struct
{
  ZCL_Request_t            unsolicitedReq;
  ZCL_OtauUpgradeEndResp_t unsolicitedUpgradeEndResp;

  uint8_t                  transacAmount;
  ZclOtauServerTransac_t  *serverTransac;
  uint16_t                 pageReminderSize;
} ZCL_OtauServerMem_t;

typedef struct
{
  ZCL_OtauStatInd_t     otauInd;
  ZCL_OtauInitParams_t  initParam;
  union
  {
    ZCL_OtauClientMem_t clientMem;
    ZCL_OtauServerMem_t serverMem;
  };
} ZclOtauMem_t;

/******************************************************************************
                           External variables section
******************************************************************************/
/** zclOtauMem is defined in zclOtauManager.c */
extern ZclOtauMem_t zclOtauMem;
extern bool isOtauBusy;

/******************************************************************************
                           Inline functions section
******************************************************************************/
INLINE ZclOtauMem_t* zclGetOtauMem(void)
{
  return &zclOtauMem;
}

INLINE ZCL_OtauClientMem_t* zclGetOtauClientMem(void)
{
  return &zclOtauMem.clientMem;
}

INLINE ZCL_OtauServerMem_t* zclGetOtauServerMem(void)
{
  return &zclOtauMem.serverMem;
}

INLINE bool isOtauStopped(void)
{
  return zclOtauMem.clientMem.isOtauStopTriggered;
}
/******************************************************************************
                   Prototypes section
******************************************************************************/
/***************************************************************************//**
\brief Lift otau cluster action for customer.

\param[in] action - the OTAU action for a customer.
******************************************************************************/
void zclRaiseCustomMessage(const ZCL_OtauAction_t action);

/***************************************************************************//**
\brief Start otau client services
******************************************************************************/
void zclStartOtauClient(void);

/***************************************************************************//**
\brief Start otau server service
******************************************************************************/
void zclStartOtauServer(void);

/***************************************************************************//**
\brief Stop otau server service
******************************************************************************/
void zclStopOtauServer(void);

/***************************************************************************//**
\brief Stop otau client service
******************************************************************************/
void zclStopOtauClient(void);

/***************************************************************************//**
\brief Clear otau server memory.
******************************************************************************/
void zclClearOtauServerMemory(void);

/***************************************************************************//**
\brief Clear otau client memory.
******************************************************************************/
void zclClearOtauClientMemory(void);

/***************************************************************************//**
\brief Find empty cell in memory pool.

\return pointer to empty cell.
******************************************************************************/
ZclOtauServerTransac_t * zclFindEmptyCell(void);

#endif /* _ZCLOTAUMANAGER_H */
