/************************************************************************//**
  \file zclOtauServer.c

  \brief
    The file implements the OTAU cluster (server part)

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.05.11 A. Khromykh - Created.
  Last change:
    $Id: zclOtauServer.c 26289 2014-02-10 14:09:50Z agasthian.s $
******************************************************************************/
#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclOTAUCluster.h>
#include <zclOtauDiscovery.h>
#include <zclOtauManager.h>
#include <zclDbg.h>
#include <zclTaskManager.h>
#include <isdImageStorage.h>

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                          Prototypes section
******************************************************************************/
static void zclIsdCommunicationStateChangedCb(ISD_Status_t status);
static void zclOtauCommonConfirm(ZCL_Notify_t *resp);
static void zclUnsolicitedReqConfirm(ZCL_Notify_t *resp);
static void zclUpgradeEndCb(ZCL_OtauUpgradeEndResp_t *resp);
static void zclImageBlockCb(ZCL_OtauImageBlockResp_t *resp);
static void zclQueryNextImageCb(ZCL_OtauQueryNextImageResp_t *resp);
static ZCL_Status_t zclUpgradeEndReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndReq_t *payload);
static ZCL_Status_t zclImageBlockReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockReq_t *payload);
static ZCL_Status_t zclQueryNextImageReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageReq_t *payload);
static void zclOtauServerHandler(void);
static void zclUnsolicitedReqConfirm(ZCL_Notify_t *resp);
static void zclOtauDefaultResponseInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static void zclOtauProcessCommonNotify(ZCL_Status_t status);
static void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload);

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  static void zclImagePageCb(ZCL_OtauImageBlockResp_t *resp);
  static ZCL_Status_t zclImagePageReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImagePageReq_t *payload);
  static void imagePageReqCallback(void);
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

/******************************************************************************
                   External global variables section
******************************************************************************/
#if defined(_SYS_ASSERT_ON_)
extern ZclOtauStateMachine_t stateMachine;
#endif // _SYS_ASSERT_ON_

/*******************************************************************************
                        Global variables section
*******************************************************************************/
// commands
ZCL_OtauClusterCommands_t otauServerCommands =
{
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  OTAU_COMMANDS(NULL, zclQueryNextImageReqInd, NULL, zclImageBlockReqInd, zclImagePageReqInd, \
                NULL, zclUpgradeEndReqInd, NULL)
#else
  OTAU_COMMANDS(NULL, zclQueryNextImageReqInd, NULL, zclImageBlockReqInd, NULL, \
                NULL, zclUpgradeEndReqInd, NULL)
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
};

DECLARE_QUEUE(zclOtauServerTransacQueue);

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Callback from image storage driver about communication state changing

\param[in] status - status of action;
******************************************************************************/
static void zclIsdCommunicationStateChangedCb(ISD_Status_t status)
{
  resetQueue(&zclOtauServerTransacQueue);
  zclClearOtauServerMemory();
  isOtauBusy = false;

  switch (status)
  {
    case ISD_COMMUNICATION_ESTABLISHED:
      zclRaiseCustomMessage(OTAU_ISD_IMAGE_CACHE_IS_FOUND);
      break;
    case ISD_HARDWARE_FAULT:
      zclRaiseCustomMessage(OTAU_ISD_HARDWARE_FAULT);
      break;
    case ISD_NO_COMMUNICATION:
      zclRaiseCustomMessage(OTAU_ISD_NO_COMMUNICATION);
      break;
    default:
      sysAssert(false, ZCL_UNKNOWN_ISD_MESSAGE);
      break;
  }
}

/***************************************************************************//**
\brief Start otau server service
******************************************************************************/
void zclStartOtauServer(void)
{
  zclClearOtauServerMemory();
  ISD_Open(zclIsdCommunicationStateChangedCb);
}

/***************************************************************************//**
\brief Stop otau server service
******************************************************************************/
void zclStopOtauServer(void)
{
  ISD_Close();
}

/***************************************************************************//**
\brief Gets Otau's cluster

\return Otau's server cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauServerCluster(void)
{
  ZCL_Cluster_t cluster = DEFINE_OTAU_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, NULL, &otauServerCommands);
  cluster.ZCL_DefaultRespInd = zclOtauDefaultResponseInd;

  return cluster;
}

/***************************************************************************//**
\brief otau server handler
******************************************************************************/
static void zclOtauServerHandler(void)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);

  if (!tmpTransac)
    return;

  switch (tmpTransac->id)
  {
    case QUERY_NEXT_IMAGE_REQUEST_ID:
      ISD_QueryNextImageReq(&tmpTransac->addressing, &tmpTransac->queryNextImageReq, zclQueryNextImageCb);
      break;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
    case IMAGE_PAGE_REQUEST_ID:
        ISD_ImageBlockReq(&tmpTransac->addressing, (ZCL_OtauImageBlockReq_t *)&tmpTransac->imagePageReq, zclImagePageCb);
      break;
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
    case IMAGE_BLOCK_REQUEST_ID:
      ISD_ImageBlockReq(&tmpTransac->addressing, &tmpTransac->imageBlockReq, zclImageBlockCb);
      break;
    case UPGRADE_END_REQUEST_ID:
      ISD_UpgradeEndReq(&tmpTransac->addressing, &tmpTransac->upgradeEndReq, zclUpgradeEndCb);
      break;
    default:
      sysAssert(false, ZCL_OTAU_UNKNOWN_SERVER_TRANSACTUION_ID);
      break;
  }
}

/***************************************************************************//**
\brief Default response handler for server zcl requests

\param[in] addressing - sender addressing information
\param[in] payloadLength - payload lenght
\param[in] payload - payload pointer
******************************************************************************/
static void zclOtauDefaultResponseInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_DefaultResp_t *defaultResp = (ZCL_DefaultResp_t*)payload;
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);

  if (!tmpTransac)
    return;

  if ((IMAGE_PAGE_REQUEST_ID == tmpTransac->id) && (IMAGE_BLOCK_RESPONSE_ID == defaultResp->commandId))
    zclOtauProcessCommonNotify(defaultResp->statusCode);

  (void)addressing;
  (void)payloadLength;
}

/***************************************************************************//**
\brief Confirm handler for server zcl requests

\param[in] resp - pointer to response
******************************************************************************/
static void zclOtauCommonConfirm(ZCL_Notify_t *resp)
{
  zclOtauProcessCommonNotify(resp->status);
}

/***************************************************************************//**
\brief Confirm or default response handler for server zcl requests

\param[in] status - confirm or Default response status
******************************************************************************/
static void zclOtauProcessCommonNotify(ZCL_Status_t status)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_OtauServerMem_t *serverMem = zclGetOtauServerMem();

  if (IMAGE_PAGE_REQUEST_ID == tmpTransac->id)
  {
    if (ZCL_SUCCESS_STATUS == status)
      if (0 != serverMem->pageReminderSize)
      {
        if (serverMem->pageReminderSize < tmpTransac->imagePageReq.maxDataSize)
          tmpTransac->imagePageReq.maxDataSize = serverMem->pageReminderSize;
        otauStartGenericTimer(tmpTransac->imagePageReq.responseSpacing, imagePageReqCallback);
        return;
      }
  }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  tmpTransac->busy = false;

  deleteHeadQueueElem(&zclOtauServerTransacQueue);

  if (getQueueElem(&zclOtauServerTransacQueue))
    zclOtauServerHandler();
  else
    isOtauBusy = false;

  (void)status;
}

/***************************************************************************//**
\brief Callback from image storage driver from Upgrade End Request

\param[in] resp - pointer to payload
******************************************************************************/
static void zclUpgradeEndCb(ZCL_OtauUpgradeEndResp_t *resp)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);

  memcpy(&tmpTransac->upgradeEndResp, resp, sizeof(ZCL_OtauUpgradeEndResp_t));

  zclOtauFillOutgoingZclRequest(UPGRADE_END_RESPONSE_ID, sizeof(ZCL_OtauUpgradeEndResp_t), (uint8_t *)&tmpTransac->upgradeEndResp);

  ZCL_CommandReq(&tmpTransac->zclCommandReq);
}

/***************************************************************************//**
\brief Callback from image storage driver from Image Block Request

\param[in] resp - pointer to payload
******************************************************************************/
static void zclImageBlockCb(ZCL_OtauImageBlockResp_t *resp)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);
  uint8_t len;

  switch (resp->status)
  {
    case ZCL_SUCCESS_STATUS:
      len = sizeof(ZCL_OtauImageBlockResp_t) - OFD_BLOCK_SIZE + resp->dataSize;
      break;
    case ZCL_WAIT_FOR_DATA_STATUS:
      len = sizeof(ZCL_Status_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t);
      break;
    default:
      len = sizeof(ZCL_Status_t);
      break;
  }

  memcpy(&tmpTransac->imageBlockResp, resp, len);
  if (ZCL_WAIT_FOR_DATA_STATUS == resp->status)
    tmpTransac->imageBlockResp.blockRequestDelay = 0;

  zclOtauFillOutgoingZclRequest(IMAGE_BLOCK_RESPONSE_ID, len, (uint8_t *)&tmpTransac->imageBlockResp);

  otauServerCommands.imageBlockResp.options.ackRequest = 1;

  ZCL_CommandReq(&tmpTransac->zclCommandReq);
}

/***************************************************************************//**
\brief Callback from image storage driver from Query Next Image Request

\param[in] resp - pointer to payload
******************************************************************************/
static void zclQueryNextImageCb(ZCL_OtauQueryNextImageResp_t *resp)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);
  uint8_t len;

  if (ZCL_SUCCESS_STATUS == resp->status)
    len = sizeof(ZCL_OtauQueryNextImageResp_t);
  else
    len = sizeof(ZCL_Status_t);

  memcpy(&tmpTransac->queryNextImageResp, resp, len);

  zclOtauFillOutgoingZclRequest(QUERY_NEXT_IMAGE_RESPONSE_ID, len, (uint8_t *)&tmpTransac->queryNextImageResp);

  ZCL_CommandReq(&tmpTransac->zclCommandReq);
}

/***************************************************************************//**
\brief Upgrade end request indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t zclUpgradeEndReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndReq_t *payload)
{
  ZclOtauServerTransac_t *tmpTransac = zclFindEmptyCell();
  (void)payloadLength;

  if (tmpTransac)
  {
    tmpTransac->busy = true;
    tmpTransac->id = UPGRADE_END_REQUEST_ID;
    tmpTransac->addressing = *addressing;
    tmpTransac->upgradeEndReq = *payload;
    putQueueElem(&zclOtauServerTransacQueue, tmpTransac);
    if (!isOtauBusy)
    {
      isOtauBusy = true;
      zclOtauServerHandler();
    }
  }

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Next image block request indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t zclImageBlockReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockReq_t *payload)
{
  ZclOtauServerTransac_t *tmpTransac = zclFindEmptyCell();
  (void)payloadLength;

  if (tmpTransac)
  {
    tmpTransac->busy = true;
    tmpTransac->id = IMAGE_BLOCK_REQUEST_ID;
    tmpTransac->addressing = *addressing;
    tmpTransac->imageBlockReq = *payload;
    putQueueElem(&zclOtauServerTransacQueue, tmpTransac);
    if (!isOtauBusy)
    {
      isOtauBusy = true;
      zclOtauServerHandler();
    }
  }

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Query next image request indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t zclQueryNextImageReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageReq_t *payload)
{
  ZclOtauServerTransac_t *tmpTransac = zclFindEmptyCell();
  (void)payloadLength;

  if (tmpTransac)
  {
    tmpTransac->busy = true;
    tmpTransac->id = QUERY_NEXT_IMAGE_REQUEST_ID;
    tmpTransac->addressing = *addressing;
    tmpTransac->queryNextImageReq = *payload;
    putQueueElem(&zclOtauServerTransacQueue, tmpTransac);
    if (!isOtauBusy)
    {
      isOtauBusy = true;
      zclOtauServerHandler();
    }
  }

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Confirm or default response handler for server zcl requests

\param[in] resp - pointer to response
******************************************************************************/
static void zclUnsolicitedReqConfirm(ZCL_Notify_t *resp)
{
  ZCL_OtauServerMem_t* serverMem = zclGetOtauServerMem();
  ZCL_Request_t *req = &serverMem->unsolicitedReq;

  req->id = 0;
  (void)resp;
}

/***************************************************************************//**
\brief The server may send an unsolicited Upgrade End Response command to the client.
This may be used for example if the server wants to synchronize the upgrade
on multiple clients simultaneously.

\param[in] addressing - pointer to structure with addressing information;
\param[in] resp - pointer to upgrade end response payload

\return true - data is sending, false - server is busy.
******************************************************************************/
bool ZCL_UnsolicitedUpgradeEndResp(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndResp_t *resp)
{
  ZCL_OtauServerMem_t* serverMem = zclGetOtauServerMem();
  ZCL_Request_t *req = &serverMem->unsolicitedReq;
  ZCL_OtauUpgradeEndResp_t *payload = &serverMem->unsolicitedUpgradeEndResp;

  if (req->id)
    return false;

  memcpy(&req->dstAddressing, addressing, sizeof(ZCL_Addressing_t));
  req->dstAddressing.sequenceNumber = ZCL_GetNextSeqNumber();
  memcpy(payload, resp, sizeof(ZCL_OtauUpgradeEndResp_t));

  req->endpointId                         = zclOtauMem.initParam.otauEndpoint;
  req->id                                 = UPGRADE_END_RESPONSE_ID;
  req->defaultResponse                    = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE;
  req->requestLength                      = sizeof(ZCL_OtauUpgradeEndResp_t);
  req->requestPayload                     = (uint8_t *)payload;
  req->ZCL_Notify                         = zclUnsolicitedReqConfirm;

  ZCL_CommandReq(req);

  return true;
}

/***************************************************************************//**
\brief Fills ZCL_Request_t structure fields for outgoing request.

\param[in] id - zcl command id;
\param[in] length - the length of zcl command payload;
\param[in] payload - pointer to zcl command payload
******************************************************************************/
static void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload)
{
  ZclOtauServerTransac_t *transac = getQueueElem(&zclOtauServerTransacQueue);

  transac->zclCommandReq.dstAddressing.addrMode             = APS_SHORT_ADDRESS;
  transac->zclCommandReq.dstAddressing.addr.shortAddress    = transac->addressing.addr.shortAddress;
  transac->zclCommandReq.dstAddressing.profileId            = transac->addressing.profileId;
  transac->zclCommandReq.dstAddressing.endpointId           = transac->addressing.endpointId;
  transac->zclCommandReq.dstAddressing.clusterId            = OTAU_CLUSTER_ID;
  transac->zclCommandReq.dstAddressing.clusterSide          = ZCL_CLIENT_CLUSTER_TYPE;
  transac->zclCommandReq.dstAddressing.manufacturerSpecCode = 0;
  transac->zclCommandReq.dstAddressing.sequenceNumber       = transac->addressing.sequenceNumber;
  transac->zclCommandReq.endpointId                         = zclOtauMem.initParam.otauEndpoint;
  transac->zclCommandReq.id                                 = id;
  transac->zclCommandReq.defaultResponse                    = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE;
  transac->zclCommandReq.requestLength                      = length;
  transac->zclCommandReq.requestPayload                     = payload;
  transac->zclCommandReq.ZCL_Notify                         = zclOtauCommonConfirm;
}

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
/***************************************************************************//**
\brief Next image page request indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t zclImagePageReqInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImagePageReq_t *payload)
{
  ZclOtauServerTransac_t *tmpTransac = zclFindEmptyCell();
  ZCL_OtauServerMem_t *serverMem = zclGetOtauServerMem();
  (void)payloadLength;
  bool pageRequestUsed;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
  if (!pageRequestUsed)
    return ZCL_UNSUP_CLUSTER_COMMAND_STATUS;

  if (tmpTransac)
  {
    tmpTransac->busy = true;
    tmpTransac->id = IMAGE_PAGE_REQUEST_ID;
    tmpTransac->addressing = *addressing;
    memcpy(&(tmpTransac->imagePageReq), payload, sizeof(ZCL_OtauImagePageReq_t));
    serverMem->pageReminderSize = payload->pageSize;
    putQueueElem(&zclOtauServerTransacQueue, tmpTransac);
    // First block response should be sent immediately after page request.
    imagePageReqCallback();
  }

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Callback from timer for Image Page Request (page spacing)
******************************************************************************/
static void imagePageReqCallback(void)
{
  if (!isOtauBusy)
    isOtauBusy = true;
  zclOtauServerHandler();
}

/***************************************************************************//**
\brief Callback from image storage driver from Image Block Request for
         Image Page Request

\param[in] resp - pointer to payload
******************************************************************************/
static void zclImagePageCb(ZCL_OtauImageBlockResp_t *resp)
{
  ZclOtauServerTransac_t *tmpTransac = getQueueElem(&zclOtauServerTransacQueue);
  ZCL_OtauServerMem_t *serverMem = zclGetOtauServerMem();
  uint8_t len;

  switch (resp->status)
  {
    case ZCL_SUCCESS_STATUS:
      len = sizeof(ZCL_OtauImageBlockResp_t) - OFD_BLOCK_SIZE + resp->dataSize;
      break;
    case ZCL_WAIT_FOR_DATA_STATUS:
      len = sizeof(ZCL_Status_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t);
      break;
    default:
      len = sizeof(ZCL_Status_t);
      break;
  }

  // First Block Response should have the same sequence number as Page Request
  // all other responses should have their own unique sequence numbers.
  tmpTransac->zclCommandReq.dstAddressing.sequenceNumber = (tmpTransac->imagePageReq.pageSize == serverMem->pageReminderSize) ?
                                                            tmpTransac->addressing.sequenceNumber :
                                                            ZCL_GetNextSeqNumber();
  serverMem->pageReminderSize -= resp->dataSize;
  tmpTransac->imagePageReq.fileOffset += resp->dataSize;

  memcpy(&tmpTransac->imageBlockResp, resp, len);
  if (ZCL_WAIT_FOR_DATA_STATUS == resp->status)
    tmpTransac->imageBlockResp.blockRequestDelay = 0;

  zclOtauFillOutgoingZclRequest(IMAGE_BLOCK_RESPONSE_ID, len, (uint8_t *)&tmpTransac->imageBlockResp);

  otauServerCommands.imageBlockResp.options.ackRequest = 0;

  ZCL_CommandReq(&tmpTransac->zclCommandReq);
}
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

// eof zclOtauServer.c

