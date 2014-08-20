#ifndef _RHRELATIVEHUMIDITYMEASUREMENTCLUSTER_H
#define _RHRELATIVEHUMIDITYMEASUREMENTCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclHumidityMeasurementCluster.h>

/******************************************************************************
                    Definition(s) section
******************************************************************************/
#define HUMIDITY_MEASUREMENT_VAL_MIN_REPORT_PERIOD 10
#define HUMIDITY_MEASUREMENT_VAL_MAX_REPORT_PERIOD 20

/******************************************************************************
                    Externals
******************************************************************************/

extern ZCL_HumidityMeasurementClusterAttributes_t  rhHumidityMeasurementClusterServerAttributes;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Temperature Measurement cluster
******************************************************************************/
void humidityMeasurementClusterInit(void);

/**************************************************************************//**
\brief Toggles Temperature
******************************************************************************/
void humidityMeasurementToggleTemperature(void);

#endif // _RHRELATIVEHUMIDITYMEASUREMENTCLUSTER_H

// eof rhRelativeHumidityMeasurementCluster.h
