/************************************************************************//**
  \file zclOtauDiscovery.c

  \brief
    The file implements the OTAU server discovery

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    13.05.10 A. Khromykh - Created.
  Last change:
    $Id: zclOtauDiscovery.c 26289 2014-02-10 14:09:50Z agasthian.s $
******************************************************************************/
#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclOTAUCluster.h>
#include <zclOtauDiscovery.h>
#include <zclOtauManager.h>
#include <zclDbg.h>
#include <configServer.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define ZERO_SERVER_EXT_ADDRESS              0x0000000000000000ull
#define COMMON_SERVER_EXT_ADDRESS            0xFFFFFFFFFFFFFFFFull

/******************************************************************************
                          Prototypes section
******************************************************************************/
static void otauMatchDescReq(void (* zdoZdpResp)(ZDO_ZdpResp_t *zdpResp));
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
static void otauLinkKeyResp(APS_RequestKeyConf_t *conf);
static void otauGetLinkKey(ExtAddr_t *tcAddr);
static void otauGetTrustCentreAddr(void);
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
static void otauServerExtAddrResp(ZDO_ZdpResp_t *zdpResp);
static void otauBroadcastMatchDescResp(ZDO_ZdpResp_t *zdpResp);
static void otauUnicastMatchDescResp(ZDO_ZdpResp_t *zdpResp);
static void otauServerShortAddrResp(ZDO_ZdpResp_t *zdpResp);
static void otauServerShortAddrReq(void);

/******************************************************************************
                   External global variables section
******************************************************************************/
#if defined(_SYS_ASSERT_ON_)
extern ZclOtauStateMachine_t stateMachine;
#endif // _SYS_ASSERT_ON_

extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;
extern QueueDescriptor_t zclOtauServerDiscoveryQueue;

/*******************************************************************************
                        Global variables section
*******************************************************************************/

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Restart poll timer

\param[in] period - timer fired time
\param[in] cb     - pointer to fired method
******************************************************************************/
void otauStartGenericTimer(uint32_t period, void (* cb)(void))
{
  if (!isOtauStopped())
  {
    ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
    HAL_AppTimer_t *tmpTimer = &clientMem->genericTimer;

    HAL_StopAppTimer(tmpTimer);
    tmpTimer->interval  = period;
    tmpTimer->mode      = TIMER_ONE_SHOT_MODE;
    tmpTimer->callback  = cb;
    HAL_StartAppTimer(tmpTimer);
  }
  else
    isOtauBusy = false;
}

/***************************************************************************//**
\brief Stop poll timer
******************************************************************************/
void otauStopGenericTimer(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  HAL_AppTimer_t *tmpTimer = &clientMem->genericTimer;

  HAL_StopAppTimer(tmpTimer);
}

/***************************************************************************//**
\brief Restart discovery service with context gap
******************************************************************************/
void otauStartPollDiscovery(void)
{
  uint32_t discoveryPeriod;

  isOtauBusy = false;
  ZCL_SET_STATE(stateMachine, OTAU_WAITING_FOR_SERVER_DISCOVERY);
  CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_SERVER_DISCOVERY_PERIOD_ID, (void *)&discoveryPeriod);
  otauStartGenericTimer(discoveryPeriod, ZCL_OtauUpgradeServerDiscovery);
}

#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
/***************************************************************************//**
\brief APS get link key response

\param conf - pointer to confirm.
******************************************************************************/
static void otauLinkKeyResp(APS_RequestKeyConf_t *conf)
{
  if (APS_RKR_SUCCESS_STATUS != conf->status)
  {
    zclRaiseCustomMessage (OTAU_SERVER_HAS_NOT_BEEN_FOUND);
    otauStartPollDiscovery();
  }
  else
  {
    zclRaiseCustomMessage (OTAU_SERVER_HAS_BEEN_FOUND);
    otauClientEntryPoint();
  }
}

/***************************************************************************//**
\brief APS link key request

\param tcAddr - pointer to trust center extended address.
******************************************************************************/
static void otauGetLinkKey(ExtAddr_t *tcAddr)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);
  APS_RequestKeyReq_t *tmpKeyReq = &clientMem->reqMem.apsKeyReq;

  COPY_EXT_ADDR(tmpKeyReq->destAddress, *tcAddr);
  tmpKeyReq->partnerAddress = tmpDiscMem->serverExtAddress;
  tmpKeyReq->keyType = APS_APP_KEY_TYPE;

  tmpKeyReq->APS_RequestKeyConf = otauLinkKeyResp;
  APS_RequestKeyReq(tmpKeyReq);
}

/***************************************************************************//**
\brief Trust center short address request
******************************************************************************/
static void otauGetTrustCentreAddr(void)
{
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  if (!APS_KEYS_FOUND(APS_FindKeys(&tmpDiscMem->serverExtAddress)))
  {
    ExtAddr_t tcAddr;

    ZCL_SET_STATE(stateMachine, OTAU_GET_LINK_KEY);
    CS_ReadParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcAddr);
    otauGetLinkKey(&tcAddr);
  }
  else
  {
    zclRaiseCustomMessage (OTAU_SERVER_HAS_BEEN_FOUND);
    otauClientEntryPoint();
  }
}
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)

/***************************************************************************//**
\brief Server IEEE addresses response

\param zdpResp - pointer to response.
******************************************************************************/
static void otauServerExtAddrResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_Status_t status;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  } 

  status = zdpResp->respPayload.status;
  if (ZDO_SUCCESS_STATUS != status)
  {
    zclRaiseCustomMessage(OTAU_SERVER_HAS_NOT_BEEN_FOUND);
    otauStartPollDiscovery();
    return;
  }

  tmpDiscMem->serverExtAddress = zdpResp->respPayload.ieeeAddrResp.ieeeAddrRemote;
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  ZCL_SET_STATE(stateMachine, OTAU_GET_TRUST_CENTRE_ADDR);
  otauGetTrustCentreAddr();
#else  // _(defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  zclRaiseCustomMessage (OTAU_SERVER_HAS_BEEN_FOUND);
  otauClientEntryPoint();
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
}

/***************************************************************************//**
\brief Server IEEE addresses request
******************************************************************************/
void otauServerExtAddrReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  tmpZdpReq->reqCluster = IEEE_ADDR_CLID;
  tmpZdpReq->dstAddrMode = APS_SHORT_ADDRESS;
  tmpZdpReq->dstAddress.shortAddress = tmpDiscMem->serverShortAddress;
  tmpZdpReq->req.reqPayload.ieeeAddrReq.nwkAddrOfInterest = tmpDiscMem->serverShortAddress;
  tmpZdpReq->req.reqPayload.ieeeAddrReq.reqType = 0;
  tmpZdpReq->ZDO_ZdpResp = otauServerExtAddrResp;

  ZDO_ZdpReq(&clientMem->reqMem.zdpOtauReq);
}

/***************************************************************************//**
\brief Response on broadcast discovery of upgrade server

\param zdpResp - pointer to response.
******************************************************************************/
static void otauBroadcastMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_Status_t status;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  uint8_t tmpAmount = clientMem->discoveredServerAmount;
  ZclOtauDiscoveryResult_t *tmpMem = clientMem->discoveredServerMem;

  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  }

  status = zdpResp->respPayload.status;
  if (ZDO_SUCCESS_STATUS == status)
  {
    if (zdpResp->respPayload.matchDescResp.matchLength)
    {
      while (tmpAmount--)
      {
        if (tmpMem->busy)
        {
          tmpMem++;
          continue;
        }

        tmpMem->busy = true;
        tmpMem->serverShortAddress = zdpResp->respPayload.matchDescResp.nwkAddrOfInterest;
        tmpMem->serverEndpoint     = zdpResp->respPayload.matchDescResp.matchList[0];
        putQueueElem(&zclOtauServerDiscoveryQueue, tmpMem);
        break;
      }
    }
  }
  else if ((ZDO_CMD_COMPLETED_STATUS == status) && (tmpMem->busy))
  {
    ZCL_SET_STATE(stateMachine, OTAU_EXT_ADDR_REQ);
    otauServerExtAddrReq();
  }
  else
  { /* there are no any servers or ZDO request failed with status of ZDO_FAIL_STATUS */
    zclRaiseCustomMessage (OTAU_SERVER_HAS_NOT_BEEN_FOUND);
    otauStartPollDiscovery();
  } /* there are no any servers or ZDO request failed with status of ZDO_FAIL_STATUS */
}

/***************************************************************************//**
\brief Response on unicast match descriptor discovery

\param[in] zdpResp - pointer to response.
******************************************************************************/
static void otauUnicastMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_Status_t status;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  }

  status = zdpResp->respPayload.status;
  if (ZDO_SUCCESS_STATUS == status)
  {
    tmpDiscMem->serverEndpoint = zdpResp->respPayload.matchDescResp.matchList[0];
  }

  if (ZDO_CMD_COMPLETED_STATUS == status)
  {
    if (tmpDiscMem->serverEndpoint)
    {
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
      ZCL_SET_STATE(stateMachine, OTAU_GET_TRUST_CENTRE_ADDR);
      otauGetTrustCentreAddr();
#else  // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
      zclRaiseCustomMessage (OTAU_SERVER_HAS_BEEN_FOUND);
      otauClientEntryPoint();
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
    }
    else
    {
      zclRaiseCustomMessage (OTAU_SERVER_HAS_NOT_BEEN_FOUND);
      otauStartPollDiscovery();
    }
  }
}

/***************************************************************************//**
\brief Send match descriptor response to know server endpoint

\param[in] zdoZdpResp - pointer to response
******************************************************************************/
static void otauMatchDescReq(void (* zdoZdpResp)(ZDO_ZdpResp_t *zdpResp))
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  tmpZdpReq->reqCluster = MATCH_DESCRIPTOR_CLID;
  tmpZdpReq->dstAddrMode = APS_SHORT_ADDRESS;

  if (tmpDiscMem)
    tmpZdpReq->dstAddress.shortAddress = tmpDiscMem->serverShortAddress;
  else
    tmpZdpReq->dstAddress.shortAddress = BROADCAST_ADDR_RX_ON_WHEN_IDLE;

  tmpZdpReq->req.reqPayload.matchDescReq.nwkAddrOfInterest = tmpZdpReq->dstAddress.shortAddress;
  tmpZdpReq->req.reqPayload.matchDescReq.profileId = zclOtauMem.initParam.profileId;
  tmpZdpReq->req.reqPayload.matchDescReq.numInClusters = 1;
  tmpZdpReq->req.reqPayload.matchDescReq.inClusterList[0] = OTAU_CLUSTER_ID;
  tmpZdpReq->req.reqPayload.matchDescReq.numOutClusters = 0;
  tmpZdpReq->ZDO_ZdpResp = zdoZdpResp;

  ZDO_ZdpReq(tmpZdpReq);
}

/***************************************************************************//**
\brief Server short address response.

\param zdpResp - pointer to response structute.
******************************************************************************/
static void otauServerShortAddrResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_Status_t status;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  }

  status = zdpResp->respPayload.status;
  if (ZDO_SUCCESS_STATUS != status)
  {
    zclRaiseCustomMessage (OTAU_SERVER_HAS_NOT_BEEN_FOUND);
    otauStartPollDiscovery();
    return;
  }

  tmpDiscMem->serverShortAddress = zdpResp->respPayload.nwkAddrResp.nwkAddrRemote;
  // get server endpoint
  ZCL_SET_STATE(stateMachine, OTAU_UNICAST_MATCH_DESC_REQ);
  otauMatchDescReq(otauUnicastMatchDescResp);
}

/***************************************************************************//**
\brief Send server short address request
******************************************************************************/
static void otauServerShortAddrReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  tmpZdpReq->reqCluster = NWK_ADDR_CLID;
  tmpZdpReq->dstAddrMode = APS_EXT_ADDRESS;
  COPY_EXT_ADDR(tmpZdpReq->dstAddress.extAddress, tmpDiscMem->serverExtAddress);
  tmpZdpReq->req.reqPayload.nwkAddrReq.ieeeAddrOfInterest = tmpDiscMem->serverExtAddress;
  tmpZdpReq->req.reqPayload.nwkAddrReq.reqType = 1;
  tmpZdpReq->ZDO_ZdpResp = otauServerShortAddrResp;

  ZDO_ZdpReq(tmpZdpReq);
}

/***************************************************************************//**
\brief Start discovery of upgrade server
******************************************************************************/
void ZCL_OtauUpgradeServerDiscovery(void)
{
  ExtAddr_t tempExtAddr;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauDiscoveryResult_t *tmpDiscMem;

  ZCL_CHECK_STATE(stateMachine, OTAU_WAITING_FOR_SERVER_DISCOVERY, ZCL_OTAU_UNEXPECTED_SERVER_DISCOVERY);

  isOtauBusy = true;
  zclRaiseCustomMessage (OTAU_SERVER_DISCOVERY_INITIATED);
  resetQueue(&zclOtauServerDiscoveryQueue);
  zclClearOtauClientMemory();
  tmpDiscMem = clientMem->discoveredServerMem;
  // write default cluster attribute value
  CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &tempExtAddr);
  COPY_EXT_ADDR(otauClientAttributes.upgradeServerIEEEAddr.value, tempExtAddr);

  if ((ZERO_SERVER_EXT_ADDRESS == tempExtAddr) || (COMMON_SERVER_EXT_ADDRESS == tempExtAddr))
  {
    // broadcast discovery
    ZCL_SET_STATE(stateMachine, OTAU_BROADCAST_MATCH_DESC_REQ);
    otauMatchDescReq(otauBroadcastMatchDescResp);
  }
  else
  {
    // unicast short address request
    ZCL_SET_STATE(stateMachine, OTAU_SHORT_ADDR_REQ);
    tmpDiscMem->busy = true;
    tmpDiscMem->serverEndpoint = 0;
    COPY_EXT_ADDR(tmpDiscMem->serverExtAddress, tempExtAddr);
    putQueueElem(&zclOtauServerDiscoveryQueue, tmpDiscMem);
    otauServerShortAddrReq();
  }
}

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

// eof zclOtauDiscovery.c
