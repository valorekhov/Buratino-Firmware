/*
 * tempHumiditySI7021.c
 *
 * Created: 8/18/2014 2:11:41 PM
 *  Author: val
 */ 


#include <buratinoSettings.h>
#if BURATINO_CAPABILITY_TEMPERATURE_SENSOR == BURATINO_PLATFORM_TEMPERATURE_SENSOR_SI7021
#include "../include/TemperatureHumidityDriver.h"
#include <i2cPacket.h>

/* Si7021 CMD Code */
#define Measure_RH_M      0xE5
#define Measure_RH_NM     0xF5
#define Measure_T_M       0xE3
#define Measure_T_M       0xF3
#define Read_Temp_RH      0xE0
#define RESET             0xFE
#define Write_RH_T_REG    0xE6
#define Read_RH_T_REG     0xE7
#define Read_ID_1_1       0xFA
#define Read_ID_1_2       0x0F
#define Read_ID_2_1       0xFC
#define Read_ID_2_2       0xC9
#define Read_Rev_1        0x84
#define Read_Rev_2        0xB8

/* ID Register */
#define ID_SI7021         0x15

#define WAKE_UP_TIME      15
#define SI7021_ADR        0x40

/* Coefficients */
#define TEMPERATURE_OFFSET   46.85
#define TEMPERATURE_MULTIPLE 175.72
#define TEMPERATURE_SLOPE    65536
#define HUMIDITY_OFFSET      6
#define HUMIDITY_MULTIPLE    125
#define HUMIDITY_SLOPE       65536


/* Variables */
static uint8_t buffer[32];
static HAL_I2cDescriptor_t i2c;

static uint8_t currentTempReading = 0;
static uint8_t currentRhReading = 0;

// Initialization steps
typedef enum {
	CLOSED = 0,
	FIRST_STEP,        // Initialize register 1
	SECOND_STEP,       // Initialize register 2
	THIRD_STEP,         // Enable READY IRQ
	OPEN
} step_t;
static step_t currentStep = CLOSED;

/* Methods */

/* last step complete, do the next one */
void doneStep(bool result)
{
	if (result)
	doStep(++currentStep);
}

/* Perform initialization step */
void doStep(step_t step)
{
	switch(currentStep) {
		case FIRST_STEP:
			buffer[0] = 0x01;
			i2c.length = 1;
			i2c.internalAddr = 0x01;
			HAL_WriteI2cPacket(&i2c);
		break;

		case SECOND_STEP:
			buffer[0] = 0x02;
			i2c.length = 1;
			i2c.internalAddr = 0x02;
			HAL_WriteI2cPacket(&i2c);
		break;

		case THIRD_STEP:
		//HAL_RegisterIrq(IRQ_6, IRQ_ANY_EDGE, sensorReadyHandler);
		//HAL_EnableIrq(IRQ_6);
		currentStep++;
		break;
	}
}

void BSP_TempHumidityInitDevice(){
	if (currentStep == CLOSED) {
		memset(0x00, &buffer[0], sizeof(buffer));
		i2c.tty = TWI_CHANNEL_0;
		i2c.clockRate = I2C_CLOCK_RATE_62;
		i2c.lengthAddr = HAL_ONE_BYTE_SIZE;
		i2c.id = SI7021_ADR;

		i2c.data = &buffer[0];
		i2c.f = doneStep;
		HAL_OpenI2cPacket(&i2c);
	}
}
//returns temp in C as an int of 0.01C precision
int16_t BSP_TempHumidityReadTemperature(){
	
}
//returns relative humidity as an int, with 0.1% precision
uint16_t BSP_TempHumidityReadRelativeHumidity(){
	return RELATIVE_HUMIDITY_READING_UNDEFINED;
}
#endif