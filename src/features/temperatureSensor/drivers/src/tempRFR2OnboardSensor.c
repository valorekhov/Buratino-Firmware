/*
 * tempRFR2OnboardSensor.c
 *
 * Created: 8/18/2014 2:11:09 PM
 *  Author: val
 */ 

#include <buratinoSettings.h>
#if BURATINO_CAPABILITY_TEMPERATURE_SENSOR == BURATINO_PLATFORM_TEMPERATURE_SENSOR_ONCHIP
#include "../include/TemperatureHumidityDriver.h"

static int16_t currentReading;

void BSP_TempHumidityInitDevice(){	
}

	uint16_t adc_meastemp (void)
	{
		ADCSRC = 10<<ADSUT0; // set start-up time
		ADCSRB = 1<<MUX5; // set MUX5 first
		ADMUX = (3<<REFS0) + (9<<MUX0); // store new ADMUX, 1.6V AREF
		// switch ADC on, set prescaler, start conversion
		ADCSRA = (1<<ADEN) + (1<<ADSC) + (4<<ADPS0);
		do
		{} while( (ADCSRA & (1<<ADSC))); // wait for conversion end
		ADCSRA = 0; // disable the ADC
		return (ADC);
	}

//returns temp in C as an int of 0.1C precision
int16_t BSP_TempHumidityReadTemperature(){
	currentReading = 11.3 * adc_meastemp() - 2728; //(1.13 * value - 272.8) * 100;
	currentReading = currentReading * 10; 
	return currentReading;	
}
//returns relative humidity as an int, with 0.1% precision
uint16_t BSP_TempHumidityReadRelativeHumidity(){
	return RELATIVE_HUMIDITY_READING_UNDEFINED;
}
#endif