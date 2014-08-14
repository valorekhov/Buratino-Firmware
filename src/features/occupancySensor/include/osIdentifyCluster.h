/**************************************************************************//**
  \file osIdentifyCluster.h

  \brief
    Occupancy Sensor Identify cluster interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.12.12 N. Fomin - Created.
******************************************************************************/
#ifndef _OSIDENTIFYCLUSTER_H
#define _OSIDENTIFYCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclIdentifyCluster.h>

/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_IdentifyClusterAttributes_t osIdentifyClusterServerAttributes;
extern ZCL_IdentifyClusterCommands_t   osIdentifyCommands;

#endif // _OSIDENTIFYCLUSTER_H

// eof osIdentifyCluster.h

