
/******************************************************************************
                    Includes section
******************************************************************************/
#include <osClusters.h>
#include <osOccupancySensingCluster.h>
#include "features/temperatureSensor/include/tmpTemperatureMeasurementCluster.h"
#include "features/temperatureSensor/include/rhRelativeHumidityMeasurementCluster.h"
#include <osBasicCluster.h>
#include <osIdentifyCluster.h> 
#include <buratinoSettings.h>

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_Cluster_t osServerClusters[BURATINO_SERVER_CLUSTERS_COUNT] =
{
  ZCL_DEFINE_BASIC_CLUSTER_SERVER(&osBasicClusterServerAttributes),
  DEFINE_IDENTIFY_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &osIdentifyClusterServerAttributes, &osIdentifyCommands),
#ifdef BURATINO_CAPABILITY_OCCUPANCY_SENSOR  
  DEFINE_OCCUPANCY_SENSING_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &osOccupancySensingClusterServerAttributes, NULL),
#endif
#ifdef BURATINO_CAPABILITY_TEMPRH_SENSOR
 ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER(&tmpTemperatureMeasurementClusterServerAttributes),
 ZCL_DEFINE_HUMIDITY_MEASUREMENT_CLUSTER_SERVER(&rhHumidityMeasurementClusterServerAttributes),
#endif
};

ZCL_Cluster_t osClientClusters[BURATINO_CLIENT_CLUSTERS_COUNT] =
{
  DEFINE_IDENTIFY_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, NULL, &osIdentifyCommands)
};

ClusterId_t osServerClusterIds[BURATINO_SERVER_CLUSTERS_COUNT] =
{
  BASIC_CLUSTER_ID,
  IDENTIFY_CLUSTER_ID,
#ifdef BURATINO_CAPABILITY_OCCUPANCY_SENSOR  
  OCCUPANCY_SENSING_CLUSTER_ID,
#endif
#ifdef BURATINO_CAPABILITY_TEMPRH_SENSOR
  TEMPERATURE_MEASUREMENT_CLUSTER_ID,
  HUMIDITY_MEASUREMENT_CLUSTER_ID
#endif
};

ClusterId_t osClientClusterIds[BURATINO_CLIENT_CLUSTERS_COUNT] =
{
  IDENTIFY_CLUSTER_ID,
#ifdef OTAU_CLIENT
  OTAU_CLUSTER_ID, // Always should be on last position in list of clusters ID.
#endif
};


// eof buratinoClusters.c
