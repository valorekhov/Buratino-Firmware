/**************************************************************************//**
\file  sysInit.c

\brief Stack initialization implementation

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    16/10/12 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysInit.h>
#if ZCL_SUPPORT == 1
  #include <zcl.h>
#endif // ZCL_SUPPORT == 1
#if defined(ZAPPSI_NP) || defined(ZAPPSI_HOST)
  #include <zsiInit.h>
  #include <zsiMem.h>
#endif // ZAPPSI_NP || ZAPPSI_HOST

/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Wrapper to initializes ZSI layer.
******************************************************************************/
void SYS_InitZsiLayer(void)
{
#if defined(ZAPPSI_NP) || defined(ZAPPSI_HOST)
  ZSI_Init();
#endif // ZAPPSI_NP || ZAPPSI_HOST
  sysStartUpdatingRandSeed();
}

/**************************************************************************//**
\brief Wrapper to initialize ZCL layer.
******************************************************************************/
void SYS_InitZclLayer(void)
{
#if ZCL_SUPPORT == 1
  ZCL_ResetReq();
#endif
}

/**************************************************************************//**
\brief Wrapper to initialize zsiMem to execute ZsiInit layer.
******************************************************************************/
void SYS_InitZsiMem(void)
{
#if defined(ZAPPSI_HOST)
  zsiMem.taskManager.pendingTasks = (1U << ZSI_INIT_TASK_ID);
#endif /* ZAPPSI_HOST */
}
//eof sysInit.c

