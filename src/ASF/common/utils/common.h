/*
 * common.h
 *
 * Created: 7/16/2014 8:39:30 PM
 *  Author: val
 */ 


#ifndef COMMON_H_
#define COMMON_H_


#include <stdint.h>
#include <avr/io.h>

struct bits {
	uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
	uint8_t b7:1;
} __attribute__((__packed__));
#define BIT(name,pin,reg) \
((*(volatile struct bits*)&reg##name).b##pin)

#define IN PIN
#define OUT PORT
#define DIR DDR

#endif /* COMMON_H_ */