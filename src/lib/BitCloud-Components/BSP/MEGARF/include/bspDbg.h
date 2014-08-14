/************************************************************************//**
  \file bspDbg.h

  \brief
    The header file describes BSP Debug Module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    10.09.21 N. FOMIN - Created.
******************************************************************************/

#ifndef _BSPDBD_H
#define _BSPDBD_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/**************************************************************************//**
                    Types section
******************************************************************************/
typedef enum
{
  BSPBUTTONS_NULLCALLBACKS_0           = 0x9000,
  BSPSENSORS_TEMPERATURENULLCALLBACK_0 = 0x9003,
  BSPSENSORS_LIGHTNULLCALLBACK_0       = 0x9004,
  BSPSENSORS_BATTERYNULLCALLBACK_0     = 0x9005,
} BspDbgCodeId_t;

#endif// _BSPDBD_H

//eof bspDbg.h
