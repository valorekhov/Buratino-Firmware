/**************************************************************************//**
  \file osBasicCluster.c

  \brief
    Occupancy Sensor Basic cluster implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.12.12 N. Fomin - Created.
******************************************************************************/
#ifdef APP_DEVICE_TYPE_OCCUPANCY_SENSOR

/******************************************************************************
                    Includes section
******************************************************************************/
#include <osBasicCluster.h>
#include <basicCluster.h>

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_BasicClusterServerAttributes_t osBasicClusterServerAttributes =
{
  ZCL_DEFINE_BASIC_CLUSTER_SERVER_ATTRIBUTES()
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Basic cluster
******************************************************************************/
void basicClusterInit(void)
{
  osBasicClusterServerAttributes.zclVersion.value          = 0x01;
  osBasicClusterServerAttributes.powerSource.value         = 0x04;
  osBasicClusterServerAttributes.physicalEnvironment.value = 0x00;
}

#endif // APP_DEVICE_TYPE_OCCUPANCY_SENSOR

// eof osBasicCluster.c
