/*******************************************************************************
 *
 * This sample has been written by Charles-Henri Hallard (hallard.me)
 *  
 * Requires bcm2835 library to be already installed
 * http://www.airspayce.com/mikem/bcm2835/
 * Requires Cayenne MQTT CPP API
 * https://github.com/myDevicesIoT/Cayenne-MQTT-CPP
 * use the Makefile in this directory:
 * make
 * sudo ./sensors
 *
 *******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "MQTTLinux.h"
#include "CayenneMQTTClient.h"

#include "i2c.h"
#include "si7021.h"
#include "bme280.h"

//Flag for Ctrl-C
volatile sig_atomic_t force_exit = 0;

#define ALTITUDE  98 // Chasseuil-du-Poitou FR
//#define ALTITUDE 118 // Montamise FR
//#define ALTITUDE 0 // To get sea-leval altitude
static char dev[16]="";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "a1ced9e0-b24e-11e6-bb76-1157597ded22";
char password[] = "0858f39268653283bf68bb08b165c07cd6cb1959";
char clientID[] = "bd6c2ab0-bd1e-11e6-9638-53ecf337e03f";

MQTTNetwork ipstack;
CayenneMQTT::MQTTClient<MQTTNetwork, MQTTTimer> mqttClient(ipstack, username, password, clientID);

// Connect to the Cayenne server.
int cayenne_connectClient(void)
{
	// Connect to the server.
	int error = 0;
	printf("Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
	if ((error = ipstack.connect(CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
		return error;
	}

	if ((error = mqttClient.connect()) != MQTT::SUCCESS) {
		ipstack.disconnect();
		return error;
	}
	printf("Connected\n");

	// Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
	mqttClient.publishData(SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
	mqttClient.publishData(SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "Linux");

	return CAYENNE_SUCCESS;
}



/* ======================================================================
Function: sig_handler
Purpose : Intercept CTRL-C keyboard to close application
Input   : signal received
Output  : -
Comments: -
====================================================================== */
void sig_handler(int sig)
{
  printf("\nBreak received, exiting!\n");
  force_exit=true;
}

/* ======================================================================
Function: main
Purpose : Main routine
Input   : -
Output  : -
Comments: -
====================================================================== */
int main ()
{
	bool has_bme280 = false;
	bool has_bmp280 = false;
	bool has_si7021 = false;
	bool has_htu21d = false;
	bool has_cayenne = false;
	uint16_t time_to_send = 0; // Allow start immediatly
	uint8_t chip_id;
	
	// caught CTRL-C to do clean-up
  signal(SIGINT, sig_handler);
    
	// Init GPIO bcm
	if (!bcm2835_init()) {
		fprintf( stderr, "bcm2835_init() Failed\n\n" );
		return 1;
	}

	if (!bcm2835_i2c_begin()) {
		fprintf( stderr, "bcm2835_i2c_begin() failed. Are you running as root??\n");
		return 1;
	}
	
	// Set I2C speed to 100KHz
	bcm2835_i2c_set_baudrate(100000);

  printf("Checking BMP280 or BME280 device...");
 	bcm2835_i2c_setSlaveAddress(BME280_I2CADDR);	
	chip_id = I2CReadReg8(BME280_CHIPID);
  //printf("Chip on I2C Address 0x%02X has ID=0x%02X\n", BME280_I2CADDR, chip_id );
  if (chip_id!=0x60 && chip_id!=0x58) {
    fprintf(stderr, "Unsupported chip ID %02X\n", chip_id );
    return 1;
  } else {
		if (chip_id == 0x58) {
			printf("BMP280, not BME280, No Humidity read\n" );
			has_bmp280 = true;
		} else {
			printf("BME280, OK!\n");
			has_bme280 = true;
		}
	}

	// Setup BMx280
	if (has_bmp280 || has_bme280) {
		bme280_read_calibration();
		
		if (chip_id == 0x60) {
			I2CWriteReg8(BME280_REGISTER_CONTROLHUMID, 0x05);// 16x oversampling 
			I2CWriteReg8(BME280_REGISTER_CONTROL, 0xB7);     // 16x ovesampling, normal mode
		} else {
			I2CWriteReg8(BME280_REGISTER_CONTROL, 0x3F);  
		}
	}

	// select SI7021/HTU21D I2C device
  printf("Checking SI7021 or HTU21D device...");
	bcm2835_i2c_setSlaveAddress	(SI7021_I2C_ADDRESS);
	uint8_t id = si7021_getID();
	// Highest resolution and check device is here
	if ( id == ID_SI7021 ) {
		strcpy(dev, "SI7021");
    printf("%s found\n", dev );
		has_si7021 = true;
	} else if ( id == ID_HTU21D ) {
		strcpy(dev, "HTU21D");
    printf("%s found\n", dev);
		has_htu21d = true;
	} else {
		strcpy(dev, "Error");
    printf("No SI7021 or HTU21D detected, check wiring\n\n");
  }
	
	// Connect to Cayenne.
	if ( cayenne_connectClient() == CAYENNE_SUCCESS)
		has_cayenne = true;
	
	do {

	// Time out expired
		if (time_to_send--==0) {
			// Reset to 5 minutes (300 sconds)
			time_to_send = 300;
			
			if (has_bmp280 || has_bme280) {
				float temp, hum, press, alt;
				bcm2835_i2c_setSlaveAddress(BME280_I2CADDR);	

				temp = bme280_read_temperature();
				hum = bme280_read_humidity();
				press = bme280_read_pressure()/100;
				alt = bme280_read_altitude();
				
				printf("%s reading\n",chip_id == 0x60 ? "BME280":"BMP280");
				printf("  %.2f C\n", temp);
				
				if (chip_id == 0x60) {
					printf("  %.2f %%RH\n", hum);
				} 
				printf("  %.2f hPa\n", press);
				printf("  %.2f m\n", alt);
				
				if (has_cayenne) {
					mqttClient.publishData(DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, temp);
					mqttClient.publishData(DATA_TOPIC, 1, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT, hum);
					mqttClient.publishData(DATA_TOPIC, 2, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, press);
					//mqttClient.publishData(DATA_TOPIC, 3, "meter" , "m", alt);
				}
			}
			
			if (has_si7021 || has_htu21d) {
				int16_t data;
				float temp=0;
				float hum = 0;

				// select SI7021 I2C device
				bcm2835_i2c_setSlaveAddress	(SI7021_I2C_ADDRESS);
				printf("%s Reading\n",has_si7021 ? "SI7021":"HTU21D");
				if (si7021_StartConv(SI7021_READ_TEMP, &data)==BCM2835_I2C_REASON_OK ) {
					temp = data / 100.0f;
					printf("  Temp : %.2fC\n", temp);
				}
				if ( si7021_StartConv(SI7021_READ_HUM, &data)==BCM2835_I2C_REASON_OK ) {
					hum = data / 100.0f;
					printf("  Hum : %.1f%%rh\n", hum);
				}

				if (has_cayenne) {
					mqttClient.publishData(DATA_TOPIC, 4, TYPE_TEMPERATURE, UNIT_CELSIUS, temp);
					mqttClient.publishData(DATA_TOPIC, 5, TYPE_RELATIVE_HUMIDITY , UNIT_PERCENT , hum);
				}

			}
		}

		// sleep 1 second
		usleep(100000);

		// Yield to allow MQTT message processing.
		mqttClient.yield(1000);
		
	}
	// Main loop until CTRL-C
	while (!force_exit && has_cayenne) ;

	printf("Exiting\n");

	// We're here because we need to exit, do it clean
	if (has_cayenne) {
		mqttClient.disconnect();
	}
	
	// Release I2C and BCM2835
	bcm2835_i2c_end();  
  bcm2835_close();	
	return 0;
}
