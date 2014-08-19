/******************************************************************************
  \file osPdt.c

  \brief
    Occupancy Sensor Persistent Data Table implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    15.11.13 N. Fomin - Created.
******************************************************************************/
//#ifdef APP_DEVICE_TYPE_OCCUPANCY_SENSOR

/******************************************************************************
                    Includes section
******************************************************************************/
#include <pdsDataServer.h>
#include <osOccupancySensingCluster.h>

/******************************************************************************
                    PDT definitions
******************************************************************************/
#ifdef _ENABLE_PERSISTENT_SERVER_
/* Occupancy Sensor application data file descriptors.
   Shall be placed in the PDS_FF code segment. */
PDS_DECLARE_FILE(APP_OS_OCCUPANCY_MEM_ID, sizeof(osOccupancySensingClusterServerAttributes.occupancy), &osOccupancySensingClusterServerAttributes.occupancy,  NO_FILE_MARKS);
//PDS_DECLARE_FILE(APP_OS_OCCUPANCY_MEM_ID, sizeof(tmpTemperatureMeasurementClusterServerAttributes.measuredValue), &tmpTemperatureMeasurementClusterServerAttributes.measuredValue,  NO_FILE_MARKS);

#endif // _ENABLE_PERSISTENT_SERVER_
//#endif // APP_DEVICE_TYPE_OCCUPANCY_SENSOR

// eof osPdt.c
