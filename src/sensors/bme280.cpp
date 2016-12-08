/*
  bme280.cpp
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

#include "i2c.h"
#include "bme280.h"

bme280_calib_data _bme280_calib;
int32_t t_fine;

void bme280_read_calibration(void)
{
	_bme280_calib.dig_T1 = I2CReadReg16LE(BME280_REGISTER_DIG_T1);
	_bme280_calib.dig_T2 = I2CReadRegS16LE(BME280_REGISTER_DIG_T2);
	_bme280_calib.dig_T3 = I2CReadRegS16LE(BME280_REGISTER_DIG_T3);

	_bme280_calib.dig_P1 = I2CReadReg16LE(BME280_REGISTER_DIG_P1);
	_bme280_calib.dig_P2 = I2CReadRegS16LE(BME280_REGISTER_DIG_P2);
	_bme280_calib.dig_P3 = I2CReadRegS16LE(BME280_REGISTER_DIG_P3);
	_bme280_calib.dig_P4 = I2CReadRegS16LE(BME280_REGISTER_DIG_P4);
	_bme280_calib.dig_P5 = I2CReadRegS16LE(BME280_REGISTER_DIG_P5);
	_bme280_calib.dig_P6 = I2CReadRegS16LE(BME280_REGISTER_DIG_P6);
	_bme280_calib.dig_P7 = I2CReadRegS16LE(BME280_REGISTER_DIG_P7);
	_bme280_calib.dig_P8 = I2CReadRegS16LE(BME280_REGISTER_DIG_P8);
	_bme280_calib.dig_P9 = I2CReadRegS16LE(BME280_REGISTER_DIG_P9);

	_bme280_calib.dig_H1 = I2CReadReg8(BME280_REGISTER_DIG_H1);
	_bme280_calib.dig_H2 = I2CReadRegS16LE(BME280_REGISTER_DIG_H2);
	_bme280_calib.dig_H3 = I2CReadReg8(BME280_REGISTER_DIG_H3);
	_bme280_calib.dig_H4 = (I2CReadReg8(BME280_REGISTER_DIG_H4) << 4) | (I2CReadReg8(BME280_REGISTER_DIG_H4+1) & 0xF);
	_bme280_calib.dig_H5 = (I2CReadReg8(BME280_REGISTER_DIG_H5+1) << 4) | (I2CReadReg8(BME280_REGISTER_DIG_H5) >> 4);
	_bme280_calib.dig_H6 = (int8_t)I2CReadReg8(BME280_REGISTER_DIG_H6);
}

float bme280_read_temperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = I2CReadReg24(BME280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
	   ((int32_t)_bme280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
	   ((int32_t)_bme280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

float bme280_read_humidity(void) {

  bme280_read_temperature(); // must be done first to get t_fine

  int32_t adc_H = I2CReadReg16(BME280_REGISTER_HUMIDDATA);

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
		  (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
	       (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
		    (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
		  ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			     ((int32_t)_bme280_calib.dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

float bme280_read_pressure(void)
{
  int64_t var1, var2, p;

  bme280_read_temperature(); // must be done first to get t_fine

  int32_t adc_P = I2CReadReg24(BME280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
  return (float)p/256;
}

/*************************************************************************
    Calculates the altitude (in meters) from the specified atmospheric
    pressure (in hPa), and sea-level pressure (in hPa).
    @param  seaLevel      Sea-level pressure in hPa
    @param  atmospheric   Atmospheric pressure in hPa
*************************************************************************/
float bme280_read_altitude(float seaLevel) 
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  // http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  float atmospheric = bme280_read_pressure() / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/*************************************************************************
    Calculates the pressure at sea level (in hPa) from the specified altitude 
    (in meters), and atmospheric pressure (in hPa).  
    @param  altitude      Altitude in meters
    @param  atmospheric   Atmospheric pressure in hPa
*************************************************************************/
float bme280_seaLevelForAltitude(float altitude, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  // http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  
  return atmospheric / pow(1.0 - (altitude/44330.0), 5.255);
}
