/*
 * TemperatureHumidityDriver.h
 *
 * Created: 8/18/2014 1:48:51 PM
 *  Author: val
 */ 


#ifndef TEMPERATUREHUMIDITYDRIVER_H_
#define TEMPERATUREHUMIDITYDRIVER_H_

#define TEMPERATURE_READING_UNDEFINED 0xFFFFL
#define RELATIVE_HUMIDITY_READING_UNDEFINED 0xFFFFL

void BSP_TempHumidityInitDevice();
//returns temp in C as an int of 0.01C precision
int16_t BSP_TempHumidityReadTemperature();
//returns relative humidity as an int, with 0.1% precision 
uint16_t BSP_TempHumidityReadRelativeHumidity();


#endif /* TEMPERATUREHUMIDITYDRIVER_H_ */