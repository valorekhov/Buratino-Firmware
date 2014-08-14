/************************************************************************//**
  \file zclOtauClient.c

  \brief
    The file implements the OTAU cluster (client part)

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.01.10 A. Khromykh - Created.
  Last change:
    $Id: zclOtauClient.c 26418 2014-02-27 14:41:24Z agasthian.s $
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
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  #include <nwkAttributes.h>
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define QUERY_NEXT_IMAGE_PERIOD              5000ul
#define WAITING_RESPONSE_TIMEOUT             60000ul
/* Timeout which OTA client waits for the first Image Block Response
  in response to Image Page Request. */
#define WAITING_BLOCK_RESPONSE_TIMEOUT       15000ul
#define REPOST_OFD_ACTION                    10ul
#define AMOUNT_MSEC_IN_SEC                   1000ul
#define WAIT_FOR_RUN_UPGRADE_REQ             0xFF
#define WAITING_UPGRADE_END_RESPONSE_TIMEOUT 3600000ul  // 1 hour (in msec)
#define UNLIMITED_WAITING_TIME               0xFFFFFFFFul

#define MESHNETICS_ID                        0x1053
#define ATMEL_ID                             0x1014
#define MANUFACTURER_ID                      ATMEL_ID

#define AUXILIARY_STRUCTURE_IS_FULL          (sizeof(uint32_t) + sizeof(uint32_t))
#define IMAGE_CRC_SIZE                       1

/******************************************************************************
                          Prototypes section
******************************************************************************/
static ZCL_Status_t queryNextImageRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload);
static ZCL_Status_t imageBlockRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload);
static ZCL_Status_t upgradeEndRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload);
static ZCL_Status_t imageNotifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload);
static void otauQueryNextImageReq(void);
static void otauImageBlockReq(void);
static void otauUpgradeEndReq(ZCL_Status_t status);
static void otauSwitchCallback(OFD_Status_t status);
static void otauFlushCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo);
static void otauWriteCallback(OFD_Status_t status);
static void otauEraseCallback(OFD_Status_t status);
static void otauLaunchServerDiscovery(void);
static void otauSomeRequestConfirm(ZCL_Notify_t *resp);
static void otauSomeDefaultResponse(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static void otauOpenOfdCallback(OFD_Status_t status);
static void otauStartSwitch(void);
static void otauStartFlush(void);
static void otauStartWrite(void);
static void otauCountActuallyDataSize(void);
static void otauStartErase(void);
static bool otauCheckBlockRespFields(ZCL_OtauImageBlockResp_t *payload);
static void otauFinalizeProcess(void);
static void otauPollServerEndUpgrade(void);
static void otauImageReq(void);
static ZCL_Status_t otauFillAuxiliaryStructure(ZCL_OtauImageBlockResp_t *payload);
static void otauBlockResponseImageDataStoring(ZCL_OtauImageBlockResp_t *payload);
static ZCL_Status_t otauProcessSuccessfullImageBlockResponse(ZCL_OtauImageBlockResp_t *payload);
static void otauStartImageLoading(ZCL_OtauQueryNextImageResp_t *payload);
static void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload);
static void otauSendQueryNextImageBasedOnQueryJitter(uint8_t jitter);
#if defined(_OTAU_DEBUG_)
void ofdInitDebugInterface(void);
#endif

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
static bool otauCheckPageIntegrity(void);
static void otauImagePageReq(void);
static void otauPageRequestTimerCallback(void);
static void otauGetMissedBlocks(void);
static void otauRestartPageRequestTimer(uint32_t period, void (* cb)(void));
static void otauCalculateTimeoutTillNextResponse(void);
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
// attributes
ZCL_OtauClusterClientAttributes_t otauClientAttributes =
{
  DEFINE_OTAU_CLIENT_ATTRIBUTES
};
// commands
ZCL_OtauClusterCommands_t otauClientCommands =
{
  OTAU_COMMANDS(imageNotifyInd, NULL, queryNextImageRespInd, NULL, NULL, \
               imageBlockRespInd, NULL, upgradeEndRespInd)
};

DECLARE_QUEUE(zclOtauServerDiscoveryQueue);

/*******************************************************************************
                        Static variables section
*******************************************************************************/
// reserved parameters to recover interrupted loading
static OtauImageAuxVar_t recoveryLoading;
static bool isServerLinkActive = false;

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Entry point to client process
******************************************************************************/
void otauClientEntryPoint(void)
{
  ZCL_ImageUpdateStatus_t upgradeStatus = otauClientAttributes.imageUpgradeStatus.value;

  if (OTAU_NORMAL == upgradeStatus)
    otauQueryNextImageReq();
  else
    otauUpgradeEndReq(ZCL_SUCCESS_STATUS);
}

/***************************************************************************//**
\brief Launch otau server discovery
******************************************************************************/
static void otauLaunchServerDiscovery(void)
{
  ZCL_OtauClientMem_t      *clientMem  = zclGetOtauClientMem();
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  /* reset upgrade status */
  otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
  clientMem->blockRequestDelayOn = OTAU_BLOCK_REQUEST_DELAY_OFF;

  tmpDiscMem->busy = false;
  deleteHeadQueueElem(&zclOtauServerDiscoveryQueue);
  tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  if (tmpDiscMem)
  {
    if (tmpDiscMem->busy)
    {
      ZCL_SET_STATE(stateMachine, OTAU_EXT_ADDR_REQ);
      otauServerExtAddrReq();
      return;
    }
  }

  /* start server discovery again */
  otauStartPollDiscovery();
}

/***************************************************************************//**
\brief All requests confirm.

\param[in] resp - pointer to response parametres.
******************************************************************************/
static void otauSomeRequestConfirm(ZCL_Notify_t *resp)
{
  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  }

  if (ZCL_SUCCESS_STATUS == resp->status)
  {
    #if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      bool pageRequestUsed;
      ZCL_Request_t *tmpZclReq = &zclGetOtauClientMem()->reqMem.zclCommandReq;

      CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
      if (pageRequestUsed && (IMAGE_PAGE_REQUEST_ID == tmpZclReq->id))
        otauRestartPageRequestTimer(WAITING_BLOCK_RESPONSE_TIMEOUT, otauPageRequestTimerCallback);
    #endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  }
  else // recieved ZCL_NO_RESPONSE_ERROR_STATUS and other
  {
    zclRaiseCustomMessage(OTAU_SERVER_HAS_BEEN_LOST);
    isServerLinkActive = false;
    otauLaunchServerDiscovery();
  }
}

/***************************************************************************//**
\brief otau default response handler.

\param[in] resp - pointer to response parametres.
******************************************************************************/
static void otauSomeDefaultResponse(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_DefaultResp_t *defaultResp = (ZCL_DefaultResp_t*)payload;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  bool pageRequestUsed = false;
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  // If it is stopped ignore this
  if (isOtauStopped())
  {
    isOtauBusy = false;
    return;
  }

  // stop waiting for indication
  HAL_StopAppTimer(&clientMem->genericTimer);

  switch (defaultResp->statusCode)
  {
    case ZCL_SUCCESS_STATUS:
      return;
    case ZCL_MALFORMED_COMMAND_STATUS:
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
      break;
    case ZCL_NO_IMAGE_AVAILABLE_STATUS:
      zclRaiseCustomMessage(OTAU_SERVER_NO_IMAGE_AVAILABLE);
      break;
    case ZCL_UNSUP_CLUSTER_COMMAND_STATUS:
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      if (IMAGE_PAGE_REQUEST_ID == tmpZclReq->id)
      {
        CS_WriteParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
        otauImageBlockReq();
        zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_UNSUPPORTED_COMMAND);
        return;
      }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_UNSUPPORTED_COMMAND);
      break;
    case ZCL_ABORT_STATUS:
      zclRaiseCustomMessage(OTAU_DOWNLOAD_ABORTED);
      break;
    default:
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_UNKNOWN_STATUS);
      break;
  }

  otauLaunchServerDiscovery();

  (void)addressing;
  (void)payloadLength;
}

/***************************************************************************//**
\brief Send indication about finalizing otau process
******************************************************************************/
static void otauFinalizeProcess(void)
{
  zclRaiseCustomMessage (OTAU_DEVICE_SHALL_CHANGE_IMAGE);
}

/***************************************************************************//**
\brief Poll otau server for upgrade end time
******************************************************************************/
static void otauPollServerEndUpgrade(void)
{
  otauUpgradeEndReq(ZCL_SUCCESS_STATUS);
}

/***************************************************************************//**
\brief Upgrade end response indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t upgradeEndRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  (void)addressing;
  (void)payloadLength;
  ZCL_Status_t status = ZCL_SUCCESS_STATUS;

  isOtauBusy = false;

  if (isOtauStopped())
    return ZCL_ABORT_STATUS;

  // stop waiting for indication
  HAL_StopAppTimer(&clientMem->genericTimer);
  if (MANUFACTURER_ID != payload->manufacturerId)
    status = ZCL_MALFORMED_COMMAND_STATUS;

  if (OTAU_SPECIFIC_IMAGE_TYPE != payload->imageType)
    status = ZCL_MALFORMED_COMMAND_STATUS;

  if (clientMem->newFirmwareVersion.memAlloc != payload->firmwareVersion.memAlloc)
    status = ZCL_MALFORMED_COMMAND_STATUS;

  if (ZCL_MALFORMED_COMMAND_STATUS == status)
  {
    otauLaunchServerDiscovery();
    return status;
  }

  if (UNLIMITED_WAITING_TIME == payload->upgradeTime)
  {
    ZCL_SET_STATE(stateMachine, OTAU_WAITING_FOR_UPGRADE_UNLIMITED);
    otauClientAttributes.imageUpgradeStatus.value = OTAU_WAITING_TO_UPGRADE;

#if defined _ENDDEVICE_
    if (DEVICE_TYPE_END_DEVICE == NWK_GetDeviceType())
      otauStartGenericTimer(WAITING_UPGRADE_END_RESPONSE_TIMEOUT, otauPollServerEndUpgrade);
#endif /* _ENDDEVICE_ */

    return ZCL_SUCCESS_STATUS;
  }

  if (payload->upgradeTime >= payload->currentTime)
  {
    ZCL_SET_STATE(stateMachine, OTAU_WAITING_FOR_UPGRADE_TIMEOUT);
    otauClientAttributes.imageUpgradeStatus.value = OTAU_COUNT_DOWN;
    otauStartGenericTimer(AMOUNT_MSEC_IN_SEC * (payload->upgradeTime - payload->currentTime), otauFinalizeProcess);
    return ZCL_SUCCESS_STATUS;
  }

  otauLaunchServerDiscovery();
  return ZCL_MALFORMED_COMMAND_STATUS;
}

/***************************************************************************//**
\brief Send upgrade end request

\param status - status of upgrade action.
******************************************************************************/
static void otauUpgradeEndReq(ZCL_Status_t status)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauUpgradeEndReq_t *tmpOtauReq = &clientMem->zclReqMem.uUpgradeEndReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;

  // If polling is happening already, no need to raise this action
  if (!zclIsOtauBusy())
    zclRaiseCustomMessage(OTAU_UPGRADE_REQUEST_INITIATED);

  ZCL_SET_STATE(stateMachine, OTAU_UPGRADE_END_TRANSAC);
  zclOtauFillOutgoingZclRequest(UPGRADE_END_REQUEST_ID, sizeof(ZCL_OtauUpgradeEndReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->status          = status;
  tmpOtauReq->manufacturerId  = MANUFACTURER_ID;
  tmpOtauReq->imageType       = OTAU_SPECIFIC_IMAGE_TYPE;
  tmpOtauReq->firmwareVersion = clientMem->newFirmwareVersion;

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Start switch between images
******************************************************************************/
static void otauStartSwitch(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  ZCL_SET_STATE(stateMachine, OTAU_SWITCH_IMAGE);
  OFD_SwitchToNewImage(OFD_POSITION_2, OFD_POSITION_1, clientMem->otauParam.imageBlockData, otauSwitchCallback);
}

/***************************************************************************//**
\brief Callback from flash driver about end of image switching

\param[in] status - status of action;
******************************************************************************/
static void otauSwitchCallback(OFD_Status_t status)
{
  ZCL_CHECK_STATE(stateMachine, OTAU_SWITCH_IMAGE, ZCL_OTAU_INVALID_OPERATION);

  switch (status)
  {
    case OFD_STATUS_SUCCESS:
      zclRaiseCustomMessage(OTAU_DOWNLOAD_FINISHED);
      // change ugrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_DOWNLOAD_COMPLETE;
      // send upgrade end request with SUCCESS
      otauUpgradeEndReq(ZCL_SUCCESS_STATUS);
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartSwitch);
      break;
    default:
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // send upgrade end request with INVALID IMAGE status. When default respose will be received
      // server discovery process will start
      otauUpgradeEndReq(ZCL_INVALID_IMAGE_STATUS);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      break;
  }
}

/***************************************************************************//**
\brief Start flush last image part
******************************************************************************/
static void otauStartFlush(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  ZCL_SET_STATE(stateMachine, OTAU_FLUSH_DATA_TO_FLASH);
  OFD_FlushAndCheckCrc(OFD_POSITION_1, clientMem->otauParam.imageBlockData, otauFlushCallback);
}

/***************************************************************************//**
\brief Callback from flash driver about end of data flushing

\param[in] status - status of action;
\param[in] imageInfo - pointer to image info
******************************************************************************/
static void otauFlushCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo)
{
  (void)imageInfo;

  ZCL_CHECK_STATE(stateMachine, OTAU_FLUSH_DATA_TO_FLASH, ZCL_OTAU_INVALID_OPERATION);

  switch (status)
  {
    case OFD_STATUS_SUCCESS:
#if defined(_OTAU_DEBUG_)
      ofdInitDebugInterface();
#else
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartSwitch);
#endif
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      // try to flush again
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartFlush);
      break;
    default:
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // send upgrade end request with INVALID IMAGE status. When default respose will be received
      // server descovery process will start
      otauUpgradeEndReq(ZCL_INVALID_IMAGE_STATUS);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      break;
  }
}

/***************************************************************************//**
\brief Start write image part
******************************************************************************/
static void otauStartWrite(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  ZCL_SET_STATE(stateMachine, OTAU_WRITE_DATA_TO_FLASH);
  OFD_Write(OFD_POSITION_1, &clientMem->memParam, otauWriteCallback);
}

/***************************************************************************//**
\brief Count current data size for data request from server
******************************************************************************/
static void otauCountActuallyDataSize(void)
{
  uint8_t size;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  clientMem->blockRequest = OTAU_PAGE_REQUEST_USAGE;

  if (tmpAuxParam->imageInternalLength > OFD_BLOCK_SIZE)
    size = OFD_BLOCK_SIZE;
  else
    size = (uint8_t)tmpAuxParam->imageInternalLength;
  tmpAuxParam->currentDataSize = size;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  if (tmpAuxParam->imageInternalLength <= OFD_BLOCK_SIZE)
  {
    clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
    return;
  }

  ZCL_OtauImagePageReq_t *tmpOtauReq = &clientMem->zclReqMem.uImagePageReq;
  uint16_t pageSize;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE_ID, &pageSize);

  if (tmpAuxParam->imageInternalLength < pageSize)
    pageSize = tmpAuxParam->imageInternalLength;
  tmpOtauReq->pageSize = pageSize;
  tmpAuxParam->lastPageSize = pageSize;
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
}

/***************************************************************************//**
\brief Callback from flash driver about end of data writing

\param[in] status - status of action;
******************************************************************************/
static void otauWriteCallback(OFD_Status_t status)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  ZCL_CHECK_STATE(stateMachine, OTAU_WRITE_DATA_TO_FLASH, ZCL_OTAU_INVALID_OPERATION);

  switch (status)
  {
    case OFD_STATUS_SUCCESS:
      // count offset for next memory transaction
      tmpMemParam->offset += tmpMemParam->length;
      if (0 == tmpAuxParam->imageInternalLength)
      { /* next image block */
        if (IMAGE_CRC_SIZE == tmpAuxParam->imageRemainder)
        { /* the end of uploading */
          otauStartGenericTimer(REPOST_OFD_ACTION, otauStartFlush);
          break;
        } /* the end of uploading */
        else
        { /* request next part of data */
          tmpAuxParam->internalAddressStatus = 0;
          tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
          if (pageRequestUsed)
          {
            clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
            otauImageBlockReq();
            break;
          }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
        } /* request next part data */
      } /* next image block */
      else
      { /* count data size for current request */
        otauCountActuallyDataSize();
      } /* count data size for current request */

      otauImageReq();
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      // try to write again
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartWrite);
      break;
    default:
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // send upgrade end request with ABORT status. When default respose will be received
      // server discovery process will start
      otauUpgradeEndReq(ZCL_ABORT_STATUS);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      break;
  }
}

/***************************************************************************//**
\brief Check fields of block response command

\param[in] payload - pointer to command.

\return true - fields are correct,  \n
        false - otherwise.
******************************************************************************/
static bool otauCheckBlockRespFields(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  if (MANUFACTURER_ID == payload->manufacturerId)
    if (OTAU_SPECIFIC_IMAGE_TYPE == payload->imageType)
      if (clientMem->newFirmwareVersion.memAlloc == payload->firmwareVersion.memAlloc)
        if (payload->dataSize <= tmpAuxParam->currentDataSize)
          if (payload->fileOffset < tmpParam->imageSize)
            return true;

  return false;
}

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
/***************************************************************************//**
\brief Check integrity of the recieved page

\return true - the whole page was received,  \n
        false - page receiving is in progress.
******************************************************************************/
static bool otauCheckPageIntegrity(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  for (uint16_t itr = 0; itr < tmpAuxParam->lastPageSize; itr++)
    if (!(clientMem->missedBytesMask[itr >> 3] & (1 << ((uint8_t)itr & 0x07))))
      return false;

  return true;
}
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

/***************************************************************************//**
\brief Processes image block response with "success" status.
******************************************************************************/
static ZCL_Status_t otauProcessSuccessfullImageBlockResponse(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  if (otauCheckBlockRespFields(payload))
  {
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
    bool pageRequestUsed;

    CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

    if (pageRequestUsed && (OTAU_PAGE_REQUEST_USAGE == clientMem->blockRequest))
    {
      /* response with wrong file offset has been received */
      if ((payload->fileOffset < tmpAuxParam->imagePageOffset) ||
          (payload->fileOffset > (tmpAuxParam->lastPageSize + tmpAuxParam->imagePageOffset)))
        return ZCL_SUCCESS_STATUS;

      if (OTAU_NOT_GET_MISSED_BYTES == clientMem->missedBytesGetting)
        otauCalculateTimeoutTillNextResponse();
      /* set up marker those bytes have been received */
      uint16_t maskOffset = (uint16_t)(payload->fileOffset - tmpAuxParam->imagePageOffset);
      uint8_t dataSize = payload->dataSize;

      for (uint16_t itr = maskOffset; itr < (maskOffset + dataSize); itr++)
        clientMem->missedBytesMask[itr >> 3] |= (0x01 << ((uint8_t)itr & 0x07));
    }
    else
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
    {
      /* Duplicate data from aps. Data has been recieved already. */
      if (payload->fileOffset != tmpAuxParam->currentFileOffset)
        return ZCL_SUCCESS_STATUS;
    }

    /* auxilliary structure filling (if it is necessary) */
    if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
      return otauFillAuxiliaryStructure(payload);

    otauBlockResponseImageDataStoring(payload);
    return ZCL_SUCCESS_STATUS;
  }
  zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
  // start server discovery again
  otauLaunchServerDiscovery();
  return ZCL_MALFORMED_COMMAND_STATUS;
}

/***************************************************************************//**
\brief Image block response indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t imageBlockRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_Status_t status;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  (void)addressing;
  (void)payloadLength;

  if (!isServerLinkActive)
    payload->status = ZCL_ABORT_STATUS;

  if (isOtauStopped())
  {
    isOtauBusy = false;
    return ZCL_ABORT_STATUS;
  }

  // stop waiting for indication
  HAL_StopAppTimer(&clientMem->genericTimer);

  // if page request mode is not supported, then initiate slow polling make otauBusy false
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

  if (!pageRequestUsed)
#endif
    isOtauBusy = false;

  switch (payload->status)
  {
    case ZCL_SUCCESS_STATUS:
      status = otauProcessSuccessfullImageBlockResponse(payload);
      break;
    case ZCL_ABORT_STATUS:
      status = ZCL_SUCCESS_STATUS;
      zclRaiseCustomMessage(OTAU_DOWNLOAD_ABORTED);
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // start server discovery again
      otauLaunchServerDiscovery();
      break;
    case ZCL_WAIT_FOR_DATA_STATUS:
    {
      uint32_t delay;
      // repeat image block request
      if (payload->requestTime < payload->currentTime)
      {
        status = ZCL_MALFORMED_COMMAND_STATUS;
        zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
        // reset upgrade status
        otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
        // start server discovery again
        otauLaunchServerDiscovery();
        break;
      }
      delay = (payload->requestTime - payload->currentTime) ? payload->requestTime - payload->currentTime : payload->blockRequestDelay;
      otauClientAttributes.minimumBlockRequestDelay.value = payload->blockRequestDelay;
      status = ZCL_SUCCESS_STATUS;
      otauStartGenericTimer(AMOUNT_MSEC_IN_SEC * delay, otauImageReq);
      clientMem->blockRequestDelayOn = OTAU_BLOCK_REQUEST_DELAY_ON;
      break;
    }
    default:
      status = ZCL_MALFORMED_COMMAND_STATUS;
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // start server discovery again
      otauLaunchServerDiscovery();
      break;
  }

  return status;
}

/***************************************************************************//**
\brief Send image block request
******************************************************************************/
static void otauImageBlockReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauImageBlockReq_t *tmpOtauReq = &clientMem->zclReqMem.uImageBlockReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  uint8_t size = clientMem->blockRequestDelayOn ? sizeof(ZCL_OtauImageBlockReq_t) : sizeof(ZCL_OtauImageBlockReq_t) - sizeof(uint16_t);

  ZCL_SET_STATE(stateMachine, OTAU_IMAGE_BLOCK_TRANSAC);
  zclOtauFillOutgoingZclRequest(IMAGE_BLOCK_REQUEST_ID, size, (uint8_t *)tmpOtauReq);

  tmpOtauReq->controlField.reqNodeIeeeAddrPresent = 0;
  tmpOtauReq->manufacturerId                      = MANUFACTURER_ID;
  tmpOtauReq->imageType                           = OTAU_SPECIFIC_IMAGE_TYPE;
  tmpOtauReq->firmwareVersion                     = clientMem->newFirmwareVersion;
  tmpOtauReq->fileOffset                          = clientMem->imageAuxParam.currentFileOffset;
  tmpOtauReq->maxDataSize                         = clientMem->imageAuxParam.currentDataSize;
  tmpOtauReq->blockRequestDelay                   = otauClientAttributes.minimumBlockRequestDelay.value;

  // enabling fast polling for block request
  if (!isOtauBusy)
  {
    zclRaiseCustomMessage(OTAU_IMAGE_BLOCK_REQ_INITIATED);
    isOtauBusy = true;
  }

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
  // save current image loading parameters
  if ((false == pageRequestUsed) || (OTAU_BLOCK_REQUEST_USAGE == clientMem->blockRequest))
#endif
  {
    recoveryLoading = clientMem->imageAuxParam;
  }

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Start erase image area
******************************************************************************/
static void otauStartErase(void)
{
  ZCL_SET_STATE(stateMachine, OTAU_ERASE_IMAGE);
  OFD_EraseImage(OFD_POSITION_1, otauEraseCallback);
}

/***************************************************************************//**
\brief Callback from flash driver about end of flash erasing

\param[in] status - status of action;
******************************************************************************/
static void otauEraseCallback(OFD_Status_t status)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  ZCL_CHECK_STATE(stateMachine, OTAU_ERASE_IMAGE, ZCL_OTAU_INVALID_OPERATION);

  switch (status)
  {
    case OFD_STATUS_SUCCESS:
      tmpAuxParam->imageInternalLength = 0ul;
      tmpAuxParam->internalAddressStatus = 0;
      tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL;
      tmpAuxParam->currentFileOffset = sizeof(ZCL_OtauSubElementHeader_t) + sizeof(ZCL_OtauUpgradeImageHeader_t);
      tmpAuxParam->imageRemainder = clientMem->otauParam.imageSize - sizeof(ZCL_OtauSubElementHeader_t) - sizeof(ZCL_OtauUpgradeImageHeader_t);
      clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
      clientMem->firstStart = OTAU_CONTINUE_CLIENT_WORK;
      otauImageBlockReq();
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      // erase memory space for new image
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartErase);
      break;
    default:
      // reset upgrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      // send upgrade end request with ABORT status. When default respose will be received
      // server descovery process will start
      otauUpgradeEndReq(ZCL_ABORT_STATUS);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      break;
  }
}

/***************************************************************************//**
\brief Check if we need to start new loading or to continue previous.
       Start process.

\param[in] payload - pointer to query next image response payload;
******************************************************************************/
static void otauStartImageLoading(ZCL_OtauQueryNextImageResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauFirmwareVersion_t version = clientMem->newFirmwareVersion;

  clientMem->newFirmwareVersion = payload->currentFirmwareVersion;
  clientMem->otauParam.imageSize = payload->imageSize;
  if ((0 == memcmp(&version, &payload->currentFirmwareVersion, sizeof(ZCL_OtauFirmwareVersion_t))) &&
      (OTAU_CONTINUE_CLIENT_WORK == clientMem->firstStart))
  { // continue uploading
    // restore current image loading parameters
    clientMem->imageAuxParam = recoveryLoading;
    otauCountActuallyDataSize();
    otauImageReq();
  } // continue uploading
  else
  { // start uploading from scratch
    // erase memory space for new image
    otauStartErase();
  } // start uploading from scratch
}

/***************************************************************************//**
\brief Query next image response indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t queryNextImageRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload)
{
  ZCL_Status_t status;
  bool queryingStatus = false;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  (void)addressing;
  (void)payloadLength;
  
  // After Resp Ind making it to slow polling
  isOtauBusy = false;
  
  if (isOtauStopped())
  {
    return ZCL_ABORT_STATUS;
  }

  // stop waiting for indication
  HAL_StopAppTimer(&clientMem->genericTimer);
  switch (payload->status)
  {
    case ZCL_SUCCESS_STATUS:
      if (MANUFACTURER_ID == payload->manufacturerId)
      {
        if (OTAU_SPECIFIC_IMAGE_TYPE == payload->imageType)
        {
          otauClientAttributes.imageUpgradeStatus.value = OTAU_DOWNLOAD_IN_PROGRESS;
          zclRaiseCustomMessage(OTAU_DOWNLOAD_STARTED);
          isServerLinkActive = true;
          // start uploading process
          otauStartImageLoading(payload);
          // doesn't need to select next server from server array
          queryingStatus = true;
          status = ZCL_SUCCESS_STATUS;
          break;
        }
      }
      status = ZCL_MALFORMED_COMMAND_STATUS;
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
      break;
    case ZCL_NO_IMAGE_AVAILABLE_STATUS:
      // start query next image request again
      otauStartGenericTimer(QUERY_NEXT_IMAGE_PERIOD, otauQueryNextImageReq);
      // doesn't need to select next server from server array
      queryingStatus = true;
      status = ZCL_SUCCESS_STATUS;
      break;
    case ZCL_NOT_AUTHORIZED_STATUS:
      status = ZCL_SUCCESS_STATUS;
      zclRaiseCustomMessage(OTAU_SERVER_IS_NOT_AUTHORIZED_TO_UPDATE);
      break;
    default:
      status = ZCL_MALFORMED_COMMAND_STATUS;
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
      break;
  }

  if (queryingStatus)
  {
    ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

    /* Saves extended address as cluster parameter. */
    otauClientAttributes.upgradeServerIEEEAddr.value = tmpDiscMem->serverExtAddress;
    /* save address of the authorized server */
    CS_WriteParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &tmpDiscMem->serverExtAddress);
  }
  else
  {
    otauLaunchServerDiscovery();
  }

  return status;
}

/***************************************************************************//**
\brief Send query next image request
******************************************************************************/
static void otauQueryNextImageReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauQueryNextImageReq_t *tmpOtauReq = &clientMem->zclReqMem.uQueryNextImageReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  
  //Enabling fast polling for Query Next Image
  if (!isOtauBusy)
  {
    zclRaiseCustomMessage(OTAU_QUERY_NEXT_IMAGE_INITIATED);
    isOtauBusy = true;
  }
  
  ZCL_SET_STATE(stateMachine, OTAU_QUERY_NEXT_IMAGE_TRANSAC);
  zclOtauFillOutgoingZclRequest(QUERY_NEXT_IMAGE_REQUEST_ID, sizeof(ZCL_OtauQueryNextImageReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->controlField.hardwareVersionPresent = 0;
  tmpOtauReq->manufacturerId                      = MANUFACTURER_ID;
  tmpOtauReq->imageType                           = OTAU_SPECIFIC_IMAGE_TYPE;
  tmpOtauReq->currentFirmwareVersion              = zclOtauMem.initParam.firmwareVersion;

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Gets Otau's cluster

\return Otau's client cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauClientCluster(void)
{
  ZCL_Cluster_t cluster = DEFINE_OTAU_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, &otauClientAttributes, &otauClientCommands);
  cluster.ZCL_DefaultRespInd = otauSomeDefaultResponse;

  return cluster;
}

/***************************************************************************//**
\brief Callback from flash driver about openning

\param[in] status - status of action;
******************************************************************************/
static void otauOpenOfdCallback(OFD_Status_t status)
{
  if (OFD_STATUS_SUCCESS == status)
  {
    // set up upgrade status
    otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
    // start server discovery
    ZCL_SET_STATE(stateMachine, OTAU_WAITING_FOR_SERVER_DISCOVERY);
    ZCL_OtauUpgradeServerDiscovery();
  }
  else
  {
    ZCL_SET_STATE(stateMachine, OTAU_STOPPED_STATE);
    zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
  }
}

/***************************************************************************//**
\brief Start otau client service
******************************************************************************/
void zclStartOtauClient(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  otauClientAttributes.upgradeServerIEEEAddr.value = APS_BROADCAST_ALL_EXT_ADDRESS;

  clientMem->firstStart = OTAU_FIRST_CLIENT_START;
  ZCL_SET_STATE(stateMachine, OTAU_OFD_INITIALIZATION_STATE);
  clientMem->isOtauStopTriggered = false;
  OFD_Open(otauOpenOfdCallback);
}

/***************************************************************************//**
\brief Stop otau client service
******************************************************************************/
void zclStopOtauClient(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  HAL_AppTimer_t *tmpTimer = &clientMem->pageRequestTimer;

  HAL_StopAppTimer(tmpTimer);
  OFD_Close();
  otauStopGenericTimer();
  clientMem->isOtauStopTriggered = true;
  isOtauBusy = false;
}

/***************************************************************************//**
\brief Decides what request should be called: page or block and calls it.
******************************************************************************/
static void otauImageReq(void)
{
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  bool pageRequestUsed;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

  if ((false == pageRequestUsed) || (OTAU_BLOCK_REQUEST_USAGE == clientMem->blockRequest))
    otauImageBlockReq();
  else
    otauImagePageReq();
#else
  otauImageBlockReq();
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
}

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
/***************************************************************************//**
\brief Send page block request
******************************************************************************/
static void otauImagePageReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauImagePageReq_t *tmpOtauReq = &clientMem->zclReqMem.uImagePageReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;

  ZCL_SET_STATE(stateMachine, OTAU_IMAGE_PAGE_TRANSAC);
  zclOtauFillOutgoingZclRequest(IMAGE_PAGE_REQUEST_ID, sizeof(ZCL_OtauImagePageReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->controlField.reqNodeIeeeAddrPresent = 0;
  tmpOtauReq->manufacturerId                      = MANUFACTURER_ID;
  tmpOtauReq->imageType                           = OTAU_SPECIFIC_IMAGE_TYPE;
  tmpOtauReq->firmwareVersion                     = clientMem->newFirmwareVersion;
  tmpOtauReq->fileOffset                          = clientMem->imageAuxParam.currentFileOffset;
  tmpOtauReq->maxDataSize                         = clientMem->imageAuxParam.currentDataSize;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, &tmpOtauReq->responseSpacing);

  clientMem->imageAuxParam.imagePageOffset = clientMem->imageAuxParam.currentFileOffset;
  tmpMemParam->length = 0;
  clientMem->missedBytesGetting = OTAU_NOT_GET_MISSED_BYTES;
  // clear mask for the lost bytes
  memset(clientMem->missedBytesMask, 0x00, tmpOtauReq->pageSize / 8 + 1);
  // save current image loading parameters
  recoveryLoading = clientMem->imageAuxParam;

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Callback function for timer used in page request
******************************************************************************/
static void otauPageRequestTimerCallback(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  clientMem->missedBytesGetting = OTAU_GET_MISSED_BYTES;
  otauGetMissedBlocks();
}

/***************************************************************************//**
\brief Sends several image block requests to get missed blocks during page request
******************************************************************************/
static void otauGetMissedBlocks(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  uint8_t *missedBytesMask = clientMem->missedBytesMask;
  uint16_t begin = 0, end;
  bool beginningFound = false;

  end = tmpAuxParam->lastPageSize;
  // looking for continious set of zeros
  for (uint16_t itr = 0; itr < tmpAuxParam->lastPageSize; itr++)
  {
    // is corresponding bit set to zero
    if (!(missedBytesMask[itr >> 3] & (1 << ((uint8_t)itr & 0x07))))
      if (!beginningFound)
      {
        begin = itr;
        beginningFound = true;
      }
    // is corresponding bit set to one after zero bit has been found
    if ((missedBytesMask[itr >> 3] & (1 << ((uint8_t)itr & 0x07))) && beginningFound)
    {
      end = itr;
      break;
    }
  }

  clientMem->imageAuxParam.currentFileOffset = clientMem->imageAuxParam.imagePageOffset + begin;

  if ((end - begin) < OFD_BLOCK_SIZE)
    clientMem->imageAuxParam.currentDataSize = end - begin;
  else
    clientMem->imageAuxParam.currentDataSize = OFD_BLOCK_SIZE;

  otauImageBlockReq();
}
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

/***************************************************************************//**
\brief Fills auxiliary structure from received block response payload

\param[in] payload - payload form received image block response

\return ZCL_SUCCESS_STATUS
******************************************************************************/
static ZCL_Status_t otauFillAuxiliaryStructure(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;

  /* fill auxiliary structure from payload */
  memcpy(&tmpParam->imageBlockData[tmpAuxParam->internalAddressStatus], payload->imageData, payload->dataSize);
  tmpAuxParam->internalAddressStatus += payload->dataSize;
  if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
  { /* auxiliary data has not filled yet */
    tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL - tmpAuxParam->internalAddressStatus;
    tmpAuxParam->currentFileOffset += payload->dataSize;
    otauImageBlockReq();
    return ZCL_SUCCESS_STATUS;
  } /* auxiliary data has not filled yet */
  else
  { /* request real image data */
    memcpy(&tmpMemParam->offset, tmpParam->imageBlockData, sizeof(uint32_t));
    memcpy(&tmpAuxParam->imageInternalLength, (tmpParam->imageBlockData + sizeof(uint32_t)), sizeof(uint32_t));
    otauCountActuallyDataSize();
    tmpAuxParam->imageRemainder -= (tmpAuxParam->imageInternalLength + AUXILIARY_STRUCTURE_IS_FULL);
  } /* request real image data */
  tmpAuxParam->currentFileOffset += payload->dataSize;
  otauImageReq();

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Stores received from image block response data to page buffer or
       to flash directly (depends on page request usage).

\param[in] payload - payload form received image block response
******************************************************************************/
static void otauBlockResponseImageDataStoring(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;

  tmpAuxParam->imageInternalLength -= payload->dataSize;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

  if (pageRequestUsed && (OTAU_PAGE_REQUEST_USAGE == clientMem->blockRequest))
  {
    memcpy(tmpParam->imagePageData + (payload->fileOffset - tmpAuxParam->imagePageOffset), payload->imageData, payload->dataSize);
    if (otauCheckPageIntegrity())
    {
      HAL_StopAppTimer(&clientMem->pageRequestTimer);
      tmpAuxParam->currentFileOffset = tmpAuxParam->imagePageOffset + tmpAuxParam->lastPageSize;
      tmpMemParam->data = tmpParam->imagePageData;
      tmpMemParam->length = tmpAuxParam->lastPageSize;
      otauStartWrite();
    }
    else
      if (OTAU_GET_MISSED_BYTES == clientMem->missedBytesGetting)
        otauGetMissedBlocks();
  }
  else
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  {
    tmpAuxParam->currentFileOffset += payload->dataSize;
    memcpy(tmpParam->imageBlockData, payload->imageData, payload->dataSize);
    tmpMemParam->data = tmpParam->imageBlockData;
    tmpMemParam->length = payload->dataSize;
    // store image to external flash
    otauStartWrite();
  }
}

/***************************************************************************//**
\brief Fills ZCL_Request_t structure fields for outgoing request.

\param[in] id - zcl command id;
\param[in] length - the length of zcl command payload;
\param[in] payload - pointer to zcl command payload
******************************************************************************/
static void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload)
{
  ZCL_Request_t *tmpZclReq = &(zclGetOtauClientMem()->reqMem.zclCommandReq);
  ZclOtauDiscoveryResult_t *tmpDiscMem = getQueueElem(&zclOtauServerDiscoveryQueue);

  sysAssert(tmpDiscMem, ZCL_OTAU_DISCOVERY_QUEUE_IS_EMPTY);
  tmpZclReq->dstAddressing.addrMode             = APS_SHORT_ADDRESS;
  tmpZclReq->dstAddressing.addr.shortAddress    = tmpDiscMem->serverShortAddress;
  tmpZclReq->dstAddressing.profileId            = zclOtauMem.initParam.profileId;
  tmpZclReq->dstAddressing.endpointId           = tmpDiscMem->serverEndpoint;
  tmpZclReq->dstAddressing.clusterId            = OTAU_CLUSTER_ID;
  tmpZclReq->dstAddressing.clusterSide          = ZCL_SERVER_CLUSTER_TYPE;
  tmpZclReq->dstAddressing.manufacturerSpecCode = 0;
  tmpZclReq->dstAddressing.sequenceNumber       = ZCL_GetNextSeqNumber();
  tmpZclReq->endpointId                         = zclOtauMem.initParam.otauEndpoint;
  tmpZclReq->id                                 = id;
  tmpZclReq->defaultResponse                    = ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE;
  tmpZclReq->requestLength                      = length;
  tmpZclReq->requestPayload                     = payload;
  tmpZclReq->responseWaitTimeout                = 0;
  tmpZclReq->ZCL_Notify                         = otauSomeRequestConfirm;
}

/***************************************************************************//**
\brief Image notify indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
static ZCL_Status_t imageNotifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload)
{
  /* Ignore Image Notifications if server isn't discovered. */
  if (!getQueueElem(&zclOtauServerDiscoveryQueue))
    return ZCL_SUCCESS_STATUS;

  if (!addressing->nonUnicast)
  {
    /* The client should always send a response to unicast image notify command,
      either the default response or the query next image request command. */
    if ((OTAU_PAYLOAD_TYPE_QUERY_JITTER != payload->payloadType) ||
        (OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE != payload->queryJitter))
      return ZCL_MALFORMED_COMMAND_STATUS;

    zclRaiseCustomMessage(OTAU_IMAGE_NOTIFICATION_RECEIVED);
    otauQueryNextImageReq();
    return ZCL_SUCCESS_STATUS;
  }

  if (OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE < payload->queryJitter)
    return ZCL_SUCCESS_STATUS;

  if (OTAU_PAYLOAD_TYPE_QUERY_JITTER == payload->payloadType)
    otauSendQueryNextImageBasedOnQueryJitter(payload->queryJitter);
  else if (OTAU_PAYLOAD_TYPE_MANUFACTURER_CODE == payload->payloadType)
  {
    if (MANUFACTURER_ID != payload->manufacturerCode)
      return ZCL_SUCCESS_STATUS;

    otauSendQueryNextImageBasedOnQueryJitter(payload->queryJitter);
  }
  else if (OTAU_PAYLOAD_TYPE_IMAGE_TYPE == payload->payloadType)
  {
    if ((MANUFACTURER_ID != payload->manufacturerCode) ||
       (OTAU_SPECIFIC_IMAGE_TYPE != payload->imageType))
      return ZCL_SUCCESS_STATUS;

    otauSendQueryNextImageBasedOnQueryJitter(payload->queryJitter);
  }
  else if (OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION == payload->payloadType)
  {
    if ((MANUFACTURER_ID != payload->manufacturerCode) ||
       (OTAU_SPECIFIC_IMAGE_TYPE != payload->imageType) ||
       (zclOtauMem.initParam.firmwareVersion.memAlloc == payload->newFileVersion))
      return ZCL_SUCCESS_STATUS;

    otauSendQueryNextImageBasedOnQueryJitter(payload->queryJitter);
  }

  (void)payloadLength; // Keep compiler happy

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Sends Query Next Image request if random jitter is equal or smaller
  than jitter

\param[in] jitter - jitter from Image Notifu payload
******************************************************************************/
static void otauSendQueryNextImageBasedOnQueryJitter(uint8_t jitter)
{
  if (SYS_GetNormalizedRandomNumber(OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE) <= jitter)
  {
    zclRaiseCustomMessage(OTAU_IMAGE_NOTIFICATION_RECEIVED);
    otauQueryNextImageReq();
  }
}

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
/***************************************************************************//**
\brief Restarts image block response timeout timer

\param[in] period - timer fired time
\param[in] cb     - pointer to fired method
******************************************************************************/
static void otauRestartPageRequestTimer(uint32_t period, void (* cb)(void))
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  HAL_AppTimer_t *tmpTimer = &clientMem->pageRequestTimer;

  HAL_StopAppTimer(tmpTimer);
  tmpTimer->interval  = period;
  tmpTimer->mode      = TIMER_ONE_SHOT_MODE;
  tmpTimer->callback  = cb;
  HAL_StartAppTimer(tmpTimer);
}

/***************************************************************************//**
\brief Calculates waiting timeout till next image page response.
******************************************************************************/
static void otauCalculateTimeoutTillNextResponse(void)
{
  uint16_t responseSpacing;
  uint32_t deliveryTime = NWK_GetUnicastDeliveryTime();

  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, &responseSpacing);
  otauRestartPageRequestTimer((deliveryTime + responseSpacing) * 2, otauPageRequestTimerCallback);
}
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

// eof zclOtauClient.c
