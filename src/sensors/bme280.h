/*
	bme280.h
	BME280 Pressure,  Temperature and Humidity sensor for Raspberry PI
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

// BMP280 default address 
#define	BME280_I2CADDR	0x76
#define	BME280_CHIPID		0xD0

enum {
	BME280_REGISTER_DIG_T1              = 0x88,
	BME280_REGISTER_DIG_T2              = 0x8A,
	BME280_REGISTER_DIG_T3              = 0x8C,

	BME280_REGISTER_DIG_P1              = 0x8E,
	BME280_REGISTER_DIG_P2              = 0x90,
	BME280_REGISTER_DIG_P3              = 0x92,
	BME280_REGISTER_DIG_P4              = 0x94,
	BME280_REGISTER_DIG_P5              = 0x96,
	BME280_REGISTER_DIG_P6              = 0x98,
	BME280_REGISTER_DIG_P7              = 0x9A,
	BME280_REGISTER_DIG_P8              = 0x9C,
	BME280_REGISTER_DIG_P9              = 0x9E,

	BME280_REGISTER_DIG_H1              = 0xA1,
	BME280_REGISTER_DIG_H2              = 0xE1,
	BME280_REGISTER_DIG_H3              = 0xE3,
	BME280_REGISTER_DIG_H4              = 0xE4,
	BME280_REGISTER_DIG_H5              = 0xE5,
	BME280_REGISTER_DIG_H6              = 0xE7,

	BME280_REGISTER_CHIPID             = 0xD0,
	BME280_REGISTER_VERSION            = 0xD1,
	BME280_REGISTER_SOFTRESET          = 0xE0,

	BME280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

	BME280_REGISTER_CONTROLHUMID       = 0xF2,
	BME280_REGISTER_CONTROL            = 0xF4,
	BME280_REGISTER_CONFIG             = 0xF5,
	BME280_REGISTER_PRESSUREDATA       = 0xF7,
	BME280_REGISTER_TEMPDATA           = 0xFA,
	BME280_REGISTER_HUMIDDATA          = 0xFD,
};

typedef struct {
uint16_t dig_T1;
      int16_t  dig_T2;
      int16_t  dig_T3;
      uint16_t dig_P1;
      int16_t  dig_P2;
      int16_t  dig_P3;
      int16_t  dig_P4;
      int16_t  dig_P5;
      int16_t  dig_P6;
      int16_t  dig_P7;
      int16_t  dig_P8;
      int16_t  dig_P9;
      uint8_t  dig_H1;
      int16_t  dig_H2;
      uint8_t  dig_H3;
      int16_t  dig_H4;
      int16_t  dig_H5;
      int8_t   dig_H6;
} bme280_calib_data;

void bme280_read_calibration(void);
float bme280_read_temperature(void);
float bme280_read_humidity(void) ;
float bme280_read_pressure(void);
float bme280_read_altitude(float seaLevelhPa = 1013.25);
float bme280_read_altitude(float seaLevel) ;
float bme280_seaLevelForAltitude(float altitude, float atmospheric);