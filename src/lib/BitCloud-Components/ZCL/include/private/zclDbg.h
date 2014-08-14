/************************************************************************//**
  \file zclDbg.h

  \brief
    The header file describes ZCL Debug Module

    The file describes the ZCL Debug Module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.12.08 A. Poptashov - Created.
******************************************************************************/

#ifndef _ZCLDBG_H
#define _ZCLDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined _SYS_ASSERT_ON_

#define  ZCL_SET_STATE(state, newState)  (state) = (newState)
#define  ZCL_CHECK_STATE(state, checkState, nameOfAssert) \
  sysAssert((checkState) == (state), nameOfAssert)

#else /* _SYS_ASSERT_ON_ */

#define  ZCL_SET_STATE(state, newState)
#define  ZCL_CHECK_STATE(state, waitState, nameOfAssert)
#if defined sysAssert
#undef sysAssert
#endif /* sysAssert */
#define  sysAssert(condition, dbgCode)  (void)0

#endif /* _SYS_ASSERT_ON_ */

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Constants section
******************************************************************************/
typedef enum
{
  /***********************************
    ZCL section. Range 0xC000 - 0xCfff
  ************************************/

  //ZCL Task Manager Id       (0xC000 - 0xC0ff)
  ZCLTASKMANAGER_TASKHANDLER_0                  = 0xC000,
  //ZCL Security Manager Id   (0xC100 - 0xC1ff)
  ZCL_STARTSECURITY_NULLREQUEST                 = 0xC100,
  ZCL_STARTSECURITY_NULLCONFIRM                 = 0xC101,
  //ZCL ZCL Id                (0xC200 - 0xC2ff)
  ZCLGETCLUSTER_0                               = 0xC200,
  PARSEDATAIND_0                                = 0xC201,
  ZCLREPORTIND_0                                = 0xC202,
  DBG_ZCL_GET_TRUST_CENTER_ADDRESS              = 0xC203,
  ZCL_UNDEFINED_CLUSTER_IN_REQUEST              = 0xC204,
  ZCL_UNEXPECTED_ASDU_LENGTH                    = 0xC205,
  UNKNOWN_DAT_TYPE_DESCR                        = 0xC206,
  ZCL_UNBOUNDED_READ                            = 0xC207,
  //ZCL Memory Manager        (0xC300 - 0xC3ff)
  ZCL_DATAIND_0                                 = 0xC300,
  ZCL_THERE_ARE_NO_BUFFERS                      = 0xC301,
  ZCL_BUFFER_SIZE_IS_TOO_LOW                    = 0xC302,
  //OTAU                      (0xC400 - 0xC4ff)
  ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE       = 0xC400,
  ZCL_OTAU_DOUBLE_START                         = 0xC401,
  ZCL_OTAU_UNEXPECTED_SERVER_DISCOVERY          = 0xC402,
  ZCL_UNKNOWN_CUSTOM_MESSAGE                    = 0xC403,
  ZCL_OTAU_UNKNOWN_SERVER_TRANSACTUION_ID       = 0xC404,
  ZCL_UNKNOWN_ISD_MESSAGE                       = 0xC405,
  ZCL_OTAU_DISCOVERY_QUEUE_IS_EMPTY             = 0xC406,
  ZCL_OTAU_INVALID_OPERATION                    = 0xC407,
  // KE
  KE_WRONG_STATE_0                              = 0xC500,
  KE_WRONG_STATE_1                              = 0xC501,
  KE_WRONG_STATE_2                              = 0xC502,
  KE_WRONG_STATE_3                              = 0xC503,
  KE_WRONG_STATE_4                              = 0xC504,
  KE_WRONG_STATE_5                              = 0xC505,
  KE_WRONG_STATE_6                              = 0xC506,
  KE_INIT_CLUSTER_0                             = 0xC507,
  KE_INIT_CLUSTER_1                             = 0xC508,
  ZCL_MEMORY_CORRUPTION_0                       = 0xC600,
  ZCL_MEMORY_CORRUPTION_1                       = 0xC601,
  ZCL_MEMORY_CORRUPTION_2                       = 0xC602,
  ZCL_MEMORY_CORRUPTION_3                       = 0xC603,

  ZCLZLLSCAN_ZCLZLLSCANREQ0                     = 0xC604,
  ZCLZLLNETWORK_ZCLZLLSTARTNETWORKREQ0          = 0xC605,
  // ZCL attributes operations
  ZCLPARSER_FORMWRITEATTRIBUTERESPONSE0         = 0xC700,
  ZCLPARSER_FORMREADATTRIBUTERESPONSE0          = 0xC701,

  ZCL_STARTSECURITY_NULLCALLBACK0               = 0xC702,
  ZCL_ZLLRESETTOFN_NULLCALLBACK0                = 0xC703,
  ZCL_ZLLSTARTNETWORK_NULLCALLBACK0             = 0xC704,
  ZCL_ZLLSTARTNETWORK_NULLCALLBACK1             = 0xC705,
  ZCL_ZLLSELECTDEVICE_NULLCALLBACK0             = 0xC706,
  ZCL_ZLLSCAN_NULLCALLBACK0                     = 0xC707,
  ZCL_ZLLSCAN_NULLCALLBACK1                     = 0xC70C,
  ZCLMEMORYMANAGER_ZCLMMGETNEXTBUSYDESCRIPTOR_0 = 0xC709,
} ZclDbgCodeId_t;

typedef enum
{
  OTAU_STOPPED_STATE,
  OTAU_STARTED_STATE,
  OTAU_OFD_INITIALIZATION_STATE,
  OTAU_BROADCAST_MATCH_DESC_REQ,
  OTAU_UNICAST_MATCH_DESC_REQ,
  OTAU_SHORT_ADDR_REQ,
  OTAU_EXT_ADDR_REQ,
  OTAU_GET_TRUST_CENTRE_ADDR,
  OTAU_GET_LINK_KEY,
  OTAU_QUERY_NEXT_IMAGE_TRANSAC,
  OTAU_WAITING_FOR_SERVER_DISCOVERY,
  OTAU_ERASE_IMAGE,
  OTAU_IMAGE_BLOCK_TRANSAC,
  OTAU_IMAGE_PAGE_TRANSAC,
  OTAU_WRITE_DATA_TO_FLASH,
  OTAU_FLUSH_DATA_TO_FLASH,
  OTAU_UPGRADE_END_TRANSAC,
  OTAU_WAITING_FOR_UPGRADE_TIMEOUT,
  OTAU_WAITING_FOR_UPGRADE_UNLIMITED,
  OTAU_SWITCH_IMAGE
} ZclOtauStateMachine_t;

#endif  //#ifndef _ZCLDBG_H

//eof zclDbg.h
