/**************************************************************************//**
  \file osOccupancySensingCluster.c

  \brief
    Occupancy Sensor Occupancy Sensing cluster implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.12.12 N. Fomin - Created.
******************************************************************************/
#include <buratinoSettings.h>
#ifdef BURATINO_CAPABILITY_OCCUPANCY_SENSOR

/******************************************************************************
                    Includes section
******************************************************************************/
#include <osOccupancySensingCluster.h>

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_OccupancySensingClusterServerAttributes_t osOccupancySensingClusterServerAttributes =
{
  ZCL_DEFINE_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES(OCCUPANCY_SENSING_VAL_MIN_REPORT_PERIOD, OCCUPANCY_SENSING_VAL_MAX_REPORT_PERIOD)
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Occupancy Sensing cluster
******************************************************************************/
void occupancySensingClusterInit(void)
{
  osOccupancySensingClusterServerAttributes.occupancySensorType.value = OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR;
  osOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;
}

/**************************************************************************//**
\brief Toggles occupancy
******************************************************************************/
void occupancySensingToggleOccupancy(void)
{
  if (OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED == osOccupancySensingClusterServerAttributes.occupancy.value)
    osOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_OCCUPIED;
  else
    osOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;

  ZCL_ReportOnChangeIfNeeded(&osOccupancySensingClusterServerAttributes.occupancy);
}

#endif // BURATINO_CAPABILITY_OCCUPANCY_SENSOR APP_DEVICE_TYPE_OCCUPANCY_SENSOR

// eof osOccupancySensingCluster.c
