/*
	i2c.h
	I2C device interface for Raspberry PI
	Charles-Henri Hallard from http://ch2i.eu

	version 1.0 2016/11/27 initial version

	Requires bcm2835 library to be already installed
  http://www.airspayce.com/mikem/bcm2835/
 
*/


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <bcm2835.h>


void 			I2CCheckError(const char * infoStr, int reasonCode);
uint16_t 	I2CReadReg16(uint8_t reg);
uint16_t 	I2CReadReg16LE(uint8_t reg); 
int16_t 	I2CReadRegS16(uint8_t reg);
int16_t 	I2CReadRegS16LE(uint8_t reg);
uint32_t 	I2CReadReg24(uint8_t reg);
uint8_t 	I2CReadReg8(uint8_t reg);
uint8_t 	I2CWriteReg8(uint8_t reg, uint8_t value);
