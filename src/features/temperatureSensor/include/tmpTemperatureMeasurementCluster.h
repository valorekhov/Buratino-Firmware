#ifndef _TMPTEMPERATUREMEASUREMENTCLUSTER_H
#define _TMPTEMPERATUREMEASUREMENTCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclTemperatureMeasurementCluster.h>

/******************************************************************************
                    Definition(s) section
******************************************************************************/
#define TEMPERATURE_MEASUREMENT_VAL_MIN_REPORT_PERIOD 10
#define TEMPERATURE_MEASUREMENT_VAL_MAX_REPORT_PERIOD 20

/******************************************************************************
                    Externals
******************************************************************************/

extern ZCL_TemperatureMeasurementClusterAttributes_t  tmpTemperatureMeasurementClusterServerAttributes;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Temperature Measurement cluster
******************************************************************************/
void temperatureMeasurementClusterInit(void);

/**************************************************************************//**
\brief Toggles Temperature
******************************************************************************/
void temperatureMeasurementToggleTemperature(void);

#endif // _TMPTEMPERATUREMEASUREMENTCLUSTER_H

// eof tmpTemperatureMeasurementCluster.h
