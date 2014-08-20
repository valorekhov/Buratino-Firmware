/*
 * tempHumiditySI7021.c
 *
 * Created: 8/18/2014 2:11:41 PM
 *  Author: val
 */ 


#include <buratinoSettings.h>
#if BURATINO_CAPABILITY_TEMPRH_SENSOR == BURATINO_PLATFORM_TEMPERATURE_SENSOR_SI7021
#include "../include/TemperatureHumidityDriver.h"
#include <halFCPU.h>
#include <compat/twi.h>
#include <halTwi.h>

#define SI7021_ADR        0x40<<1
#define BSP_UID_I2C_PRESCALER     0

/* Coefficients */
#define TEMPERATURE_OFFSET   46.85
#define TEMPERATURE_MULTIPLE 175.72
#define TEMPERATURE_SLOPE    65536
#define HUMIDITY_OFFSET      6
#define HUMIDITY_MULTIPLE    125
#define HUMIDITY_SLOPE       65536


#define TWI_PRE     1        // my TWI prescaler value
#define TWI_FREQ    100000   // my TWI bit rate

/* Variables */
static uint32_t currentTempReading = 0;
static uint32_t currentRhReading = RELATIVE_HUMIDITY_READING_UNDEFINED;

uint8_t twi_poll (uint8_t u)
{
	TWCR = u | _BV(TWINT) | _BV(TWEN);  // initiate a TWI transmission
	if (u == _BV(TWSTO)) return 0;      // don't poll if the STOP condition was sent
	while ((TWCR & _BV(TWINT)) == 0) ;      // wait for the interrupt flag to be set
	return (TWSR & 0xf8);               // return the status code
}

uint8_t twi_receive16  (uint8_t sla, uint8_t cmd, uint8_t *msb, uint8_t *lsb)
{
	uint8_t u, status = 0;
	u = twi_poll(_BV(TWSTA));                    // send START
	if (u != TW_START && u != TW_REP_START) return 0;
	TWDR = sla | TW_WRITE;                       // send SLA+W
	u = twi_poll(0);
	if (u != TW_MT_SLA_ACK) goto release;
	TWDR = cmd;                                  // send command
	if (twi_poll(0) != TW_MT_DATA_ACK) goto release;
	if (twi_poll(_BV(TWSTA)) != TW_REP_START) goto release; // send RESTART
	TWDR = sla | TW_READ;                        // send SLA+R
	if (twi_poll(0) != TW_MR_SLA_ACK) goto release;
	if (twi_poll(_BV(TWEA)) != TW_MR_DATA_ACK) goto release;
	*msb = TWDR;                                 // read msb
	if (twi_poll(0) != TW_MR_DATA_NACK) goto release;
	*lsb = TWDR;                                 // read lsb
	status = 1;                                  // success
release:
	twi_poll(_BV(TWSTO));                        // send STOP
	return status;
}

void twiOn(){
	
	TWCR = 0x00;
	TWSR = BSP_UID_I2C_PRESCALER; // prescaler
	// Set 250 Kb/s clock rate
	TWBR = ((F_CPU/62500ul) - 16ul)/(2ul * (1ul << HAL_I2C_PRESCALER) * (1ul << HAL_I2C_PRESCALER));
	TWCR &= (~(1 << TWIE));
}

#define twiOff() TWCR = 0x00;

void BSP_TempHumidityInitDevice(){	
}


//returns temp in C as an int of 0.01C precision
int16_t BSP_TempHumidityReadTemperature(){
	uint8_t msb,lsb;
	//Instruct the chip to reuse temp value from prior RH measurement (if one was performed, indicated by currentRhReading != 0xFF) using 0xE0 command
	//	or, initiate a new temp reading by the 0xE3 command
	twiOn();
	if (twi_receive16(SI7021_ADR, currentRhReading != RELATIVE_HUMIDITY_READING_UNDEFINED  ? 0xE0 :  0xE3, &msb, &lsb)){
		currentTempReading = msb << 8 | lsb; 	
		currentTempReading = ((currentTempReading * TEMPERATURE_MULTIPLE ) / TEMPERATURE_SLOPE - TEMPERATURE_OFFSET) * 100; 		
	}
	else{
		currentTempReading = TEMPERATURE_READING_UNDEFINED;
	}
	twiOff();
	return currentTempReading;
}
//returns relative humidity as an int, with 0.01% precision
uint16_t BSP_TempHumidityReadRelativeHumidity(){
	uint8_t msb,lsb;
	twiOn();
	if (twi_receive16(SI7021_ADR, 0xE5, &msb, &lsb)){
		currentRhReading = msb << 8 | lsb;
		currentRhReading = currentRhReading >= 0xFFF0 ? 0x2710 : ((currentRhReading * HUMIDITY_MULTIPLE ) / HUMIDITY_SLOPE  - HUMIDITY_OFFSET) * 100;		
	} else{
		currentRhReading = RELATIVE_HUMIDITY_READING_UNDEFINED;
	}
	twiOff();
	return currentRhReading;
}

#endif