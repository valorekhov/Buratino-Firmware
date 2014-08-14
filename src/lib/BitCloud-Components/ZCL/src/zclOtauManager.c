/************************************************************************//**
  \file zclOtauManager.c

  \brief
    The file implements the common OTAU stuffs (task handler etc)

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.05.11 A. Khromykh - Created.
  Last change:
    $Id: zclOtauManager.c 26105 2014-01-17 13:51:34Z nikita.fomin $
******************************************************************************/
#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclOTAUCluster.h>
#include <zclOtauDiscovery.h>
#include <zclDbg.h>
#include <zclOtauManager.h>

/******************************************************************************
                   Local variables section
******************************************************************************/
static bool isOtaStarted;

/******************************************************************************
                   Global variables section
******************************************************************************/
#if defined(_SYS_ASSERT_ON_)
ZclOtauStateMachine_t stateMachine = OTAU_STOPPED_STATE;
#endif // _SYS_ASSERT_ON_
ZclOtauMem_t zclOtauMem;
bool isOtauBusy;

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Checks whether OTAU cluster is busy or not.

\return true - if OTAU cluster is busy, false - otherwise.
******************************************************************************/
bool zclIsOtauBusy(void)
{
  return isOtauBusy;
}

/***************************************************************************//**
\brief Find empty cell into memory pool.

\return pointer to empty cell.
******************************************************************************/
ZclOtauServerTransac_t * zclFindEmptyCell(void)
{
  ZCL_OtauServerMem_t *serverMem = zclGetOtauServerMem();
  uint8_t transacAmount = serverMem->transacAmount;
  ZclOtauServerTransac_t *tmpTransac = serverMem->serverTransac;

  while (transacAmount--)
  {
    if (tmpTransac->busy)
    {
      tmpTransac++;
      continue;
    }

    return tmpTransac;
  }

  return NULL;
}

/***************************************************************************//**
\brief OTAU parametres initialization. Start server discovery procedure
 on client side or start server.

\param[in] pInitParam - pointer to init structure.
\param[in] ind        - indication for OTAU actions \n
("ind" can be NULL if indication is not required.)
******************************************************************************/
ZCL_Status_t ZCL_StartOtauService(ZCL_OtauInitParams_t *pInitParam, ZCL_OtauStatInd_t ind)
{
  sysAssert(pInitParam != NULL, ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE);
  if (isOtaStarted)
  {
    zclRaiseCustomMessage(OTAU_DOUBLE_START);
    return ZCL_SUCCESS_STATUS;
  }

  zclOtauMem.initParam = *pInitParam;
  zclOtauMem.otauInd = ind;
  ZCL_SET_STATE(stateMachine, OTAU_STARTED_STATE);
  isOtaStarted = true;

  if (ZCL_CLIENT_CLUSTER_TYPE == pInitParam->clusterSide)
    zclStartOtauClient();
  else
    zclStartOtauServer();

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Stop OTAU service. If image uploading was in progress, than process is finished.

\param[in] pInitParam - pointer to init structure.

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StopOtauService(ZCL_OtauInitParams_t *pInitParam)
{
  sysAssert(pInitParam != NULL, ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE);
  ZCL_SET_STATE(stateMachine, OTAU_STOPPED_STATE);
  isOtaStarted = false;

  if (ZCL_SERVER_CLUSTER_TYPE == pInitParam->clusterSide)
    zclStopOtauServer();
  else
    zclStopOtauClient();

  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Lift otau cluster for customer.

\param[in] action - the OTAU action for a customer.
******************************************************************************/
void zclRaiseCustomMessage(const ZCL_OtauAction_t action)
{
  sysAssert(action < OTAU_MAX_MESSAGE_NUMBER, ZCL_UNKNOWN_CUSTOM_MESSAGE);

  if (zclOtauMem.otauInd)
    zclOtauMem.otauInd(action);
}

/***************************************************************************//**
\brief Clear otau server memory.
******************************************************************************/
void zclClearOtauServerMemory(void)
{
  ZCL_OtauServerMem_t *serverMem = zclGetOtauServerMem();

  CS_ReadParameter(CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT_ID, &serverMem->transacAmount);
  CS_GetMemory(CS_ZCL_OTAU_CLIENT_SESSION_MEMORY_ID, (void *)&serverMem->serverTransac);
  memset(serverMem->serverTransac, 0x00, sizeof(ZclOtauServerTransac_t) * serverMem->transacAmount);
}

/***************************************************************************//**
\brief Clear otau client memory.
******************************************************************************/
void zclClearOtauClientMemory(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  CS_ReadParameter(CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT_ID, &clientMem->discoveredServerAmount);
  CS_GetMemory(CS_ZCL_OTAU_DISCOVERED_SERVER_RESULT_ID, (void *)&clientMem->discoveredServerMem);
  memset(clientMem->discoveredServerMem, 0x00, sizeof(ZclOtauDiscoveryResult_t) * clientMem->discoveredServerAmount);

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  CS_GetMemory(CS_ZCL_OTAU_MISSED_BYTES_MASK_ID, (void *)&clientMem->missedBytesMask);

  CS_GetMemory(CS_ZCL_OTAU_PAGE_REQUEST_PAGE_BUFFER_ID, (void *)&clientMem->otauParam.imagePageData);
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
}

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

// eof zclOtauManager.c

