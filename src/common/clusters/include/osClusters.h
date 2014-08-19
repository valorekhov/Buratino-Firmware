/**************************************************************************//**
  \file osClusters.h

  \brief
    Occupancy Sensor clusters interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.12.12 N. Fomin - Created.
******************************************************************************/
#ifndef _OSCLUSTER_H
#define _OSCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl.h>
#include <buratinoSettings.h>

/******************************************************************************
                    Definitions section
******************************************************************************/


/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_Cluster_t osServerClusters[BURATINO_SERVER_CLUSTERS_COUNT];
extern ClusterId_t   osServerClusterIds[BURATINO_SERVER_CLUSTERS_COUNT];

extern ZCL_Cluster_t osClientClusters[BURATINO_CLIENT_CLUSTERS_COUNT];
extern ClusterId_t   osClientClusterIds[BURATINO_CLIENT_CLUSTERS_COUNT];

#endif // _OSCLUSTER_H

// eof osClusters.h
