#include <buratinoSettings.h>
#ifdef BURATINO_CAPABILITY_TEMPRH_SENSOR

/*
	The file ties together RH and temperature sensing clusters
	This is done because most sensors will perform and store temp measurement as a part of RH measurement and to avoid
	additional spend of energy on dedicated ADC conversions
*/

/******************************************************************************
                    Includes section
******************************************************************************/
#include "../include/tmpTemperatureMeasurementCluster.h"
#include "../include/rhRelativeHumidityMeasurementCluster.h"
#include "../drivers/include/TemperatureHumidityDriver.h"

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_TemperatureMeasurementClusterAttributes_t tmpTemperatureMeasurementClusterServerAttributes =
{
  ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(HUMIDITY_MEASUREMENT_VAL_MIN_REPORT_PERIOD, HUMIDITY_MEASUREMENT_VAL_MAX_REPORT_PERIOD)
};

ZCL_HumidityMeasurementClusterAttributes_t  rhHumidityMeasurementClusterServerAttributes =
{	
	ZCL_DEFINE_HUMIDITY_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(HUMIDITY_MEASUREMENT_VAL_MIN_REPORT_PERIOD, HUMIDITY_MEASUREMENT_VAL_MAX_REPORT_PERIOD)
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Temperature Measurement & RH clusters
******************************************************************************/
void temperatureRhMeasurementClustersInit(void)
{
	//BSP_TempHumidityInitDevice();
	tmpTemperatureMeasurementClusterServerAttributes.measuredValue.value = 0xffff;
	tmpTemperatureMeasurementClusterServerAttributes.minMeasuredValue.value = 0xffff;
	tmpTemperatureMeasurementClusterServerAttributes.maxMeasuredValue.value = 0xffff;
	rhHumidityMeasurementClusterServerAttributes.measuredValue.value = 0xffff;
	rhHumidityMeasurementClusterServerAttributes.minMeasuredValue.value = 0xffff;
	rhHumidityMeasurementClusterServerAttributes.maxMeasuredValue.value = 0xffff;
}


/**************************************************************************//**
\brief Toggles Temperature
******************************************************************************/
void tempRhMeasurementToggleTemperatureAndHumidity(void)
{	
	//RH reading has to happen first in order to avoid second ADC operation for temp conversion
	rhHumidityMeasurementClusterServerAttributes.measuredValue.value = BSP_TempHumidityReadRelativeHumidity();
	//rhHumidityMeasurementClusterServerAttributes.measuredValue.value = 0x2710;
	tmpTemperatureMeasurementClusterServerAttributes.measuredValue.value = BSP_TempHumidityReadTemperature();
	ZCL_ReportOnChangeIfNeeded(&tmpTemperatureMeasurementClusterServerAttributes.measuredValue);
	ZCL_ReportOnChangeIfNeeded(&rhHumidityMeasurementClusterServerAttributes.measuredValue);
}

#endif //BURATINO_CAPABILITY_TEMPRH_SENSOR

// eof tmpTemperatureMeasurementCluster.c
