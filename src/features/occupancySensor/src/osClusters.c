/**************************************************************************//**
  \file osClusters.c

  \brief
    Occupancy Sensor clusters implementation.

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
#include <osClusters.h>
#include <osOccupancySensingCluster.h>
#include <osBasicCluster.h>
#include <osIdentifyCluster.h> 

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_Cluster_t osServerClusters[OS_SERVER_CLUSTERS_COUNT] =
{
  ZCL_DEFINE_BASIC_CLUSTER_SERVER(&osBasicClusterServerAttributes),
  DEFINE_IDENTIFY_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &osIdentifyClusterServerAttributes, &osIdentifyCommands),
  DEFINE_OCCUPANCY_SENSING_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &osOccupancySensingClusterServerAttributes, NULL)
};

ZCL_Cluster_t osClientClusters[OS_CLIENT_CLUSTERS_COUNT] =
{
  DEFINE_IDENTIFY_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, &osIdentifyCommands)
};

ClusterId_t osServerClusterIds[OS_SERVER_CLUSTERS_COUNT] =
{
  BASIC_CLUSTER_ID,
  IDENTIFY_CLUSTER_ID,
  OCCUPANCY_SENSING_CLUSTER_ID,
};

ClusterId_t osClientClusterIds[OS_CLIENT_CLUSTERS_COUNT] =
{
  IDENTIFY_CLUSTER_ID,
#ifdef OTAU_CLIENT
  OTAU_CLUSTER_ID, // Always should be on last position in list of clusters ID.
#endif
};

#endif // APP_DEVICE_TYPE_OCCUPANCY_SENSOR

// eof osClusters.c
