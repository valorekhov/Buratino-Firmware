#include <buratinoSettings.h>
#ifdef BURATINO_CAPABILITY_TEMPERATURE_SENSOR

/******************************************************************************
                    Includes section
******************************************************************************/
#include "../include/tmpTemperatureMeasurementCluster.h"
#include "../drivers/include/TemperatureHumidityDriver.h"

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_TemperatureMeasurementClusterAttributes_t tmpTemperatureMeasurementClusterServerAttributes =
{
  ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(TEMPERATURE_MEASUREMENT_VAL_MIN_REPORT_PERIOD, TEMPERATURE_MEASUREMENT_VAL_MAX_REPORT_PERIOD)
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Temperature Measurement cluster
******************************************************************************/
void temperatureMeasurementClusterInit(void)
{
	BSP_TempHumidityInitDevice();
	tmpTemperatureMeasurementClusterServerAttributes.measuredValue.value = 0xFFFF;
}

/**************************************************************************//**
\brief Toggles Temperature
******************************************************************************/
void temperatureMeasurementToggleTemperature(void)
{
	tmpTemperatureMeasurementClusterServerAttributes.measuredValue.value = BSP_TempHumidityReadTemperature();
	ZCL_ReportOnChangeIfNeeded(&tmpTemperatureMeasurementClusterServerAttributes.measuredValue);
}

#endif //BURATINO_CAPABILITY_TEMPERATURE_SENSOR

// eof tmpTemperatureMeasurementCluster.c
