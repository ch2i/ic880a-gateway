/*
	i2c.c
	I2C device interface for Raspberry PI
	Charles-Henri Hallard from http://ch2i.eu

	version 1.0 2016/11/27 initial version

	Requires bcm2835 library to be already installed
  http://www.airspayce.com/mikem/bcm2835/
 
*/

#include "i2c.h"

void I2CCheckError(const char * infoStr, int reasonCode)
{
	if (reasonCode != BCM2835_I2C_REASON_OK) {
    fprintf(stderr, "Error %s =>", infoStr);
    if ( reasonCode == BCM2835_I2C_REASON_ERROR_NACK ) {
			fprintf(stderr, "Received NACK\n");
		} else if ( reasonCode == BCM2835_I2C_REASON_ERROR_CLKT ) {
			fprintf(stderr, "Received Clock Stretch Timeout\n");
		} else if ( reasonCode == BCM2835_I2C_REASON_ERROR_DATA ) {
			fprintf(stderr, "Not all data is sent/received\n");
		} else {
			fprintf(stderr, "Unknown!\n");
		}
	}
}

// Unsigned Big Indian
uint16_t I2CReadReg16(uint8_t reg)
{
	char buf[2];
	int ret = bcm2835_i2c_read_register_rs(  (char *) &reg, buf, 2);
	I2CCheckError("I2CReadReg16()", ret );
  return ( (buf[0]<<8) | buf[1] );
}

// Unsigned Low Indian
uint16_t I2CReadReg16LE(uint8_t reg) 
{
  int temp = I2CReadReg16(reg);
  return (temp >> 8) | (temp << 8);
}


// Signed Big Indian
int16_t I2CReadRegS16(uint8_t reg)
{
  return (int16_t)I2CReadReg16(reg);
}

// Signed Low Indian
int16_t I2CReadRegS16LE(uint8_t reg)
{
  return (int16_t)I2CReadReg16LE(reg);
}

// Unsigned Big Indian 24 bit
uint32_t I2CReadReg24(uint8_t reg)
{
	char buf[3];
	int ret = bcm2835_i2c_read_register_rs(  (char *) &reg, buf, 3);
	I2CCheckError("I2CReadReg24()", ret );
  return ( (buf[0]<<16) | (buf[1]<<8) | buf[2] );
}


uint8_t I2CReadReg8(uint8_t reg)
{
	char buf[1];
	int ret = bcm2835_i2c_read_register_rs( (char *) &reg, buf, 1);
	I2CCheckError("I2CReadReg8()", ret );
	return (int) buf[0];
}

uint8_t I2CWriteReg8(uint8_t reg, uint8_t value)
{
	char buf[2];
	buf[0] = reg;
	buf[1] = value;
	
	int ret = bcm2835_i2c_write( buf, sizeof(buf));
	I2CCheckError("I2CWriteReg8()", ret );
	return (int) ret;
}

