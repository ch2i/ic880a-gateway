# The Things Network: iC880a-based gateway

Reference setup for [The Things Network](http://thethingsnetwork.org/) gateways based on the iC880a USB concentrator with a Raspberry Pi host.

This installer targets the **[CH2i SPI Raspberry PI shield version](https://github.com/ch2i/iC880A-Raspberry-PI)** of the board.    

<img src="https://raw.githubusercontent.com/ch2i/ic880a-gateway/ch2i-rpi-shield/pictures/ch2i-shield-mounted.jpg" alt="Full stack" width="25%" height="25%">&nbsp;<img src="https://raw.githubusercontent.com/ch2i/ic880a-gateway/ch2i-rpi-shield/pictures/ch2i-shield-lorawan-gateway.jpg" alt="CH2i LoraWan Gateway into nice enclosure" width="25%" height="25%"> 

if you have the classic SPI wiring version, [check this branch](https://github.com/ttn-zh/ic880a-gateway/tree/spi).
if you have the USB version, [check this branch](https://github.com/ttn-zh/ic880a-gateway/).

## Setup based on Raspbian image and Raspberry PI shield Version 1.1 or greater

- Download [Raspbian Jessie Lite](https://www.raspberrypi.org/downloads/)
- Follow the [installation instruction](https://www.raspberrypi.org/documentation/installation/installing-images/README.md) to create the SD card
- Plug the iC880a on the shield then the shield on the Raspberry PI
- Connect your RPi with Ethernet cable (WiFi could be used after first setup)
- Bring power to the whole thing using using one of following options
  - **Single Shared power** between Raspberry PI and iC880A board (JP1 on shield need to be closed)
    - Descent 5V power (2A) via Raspberry PI microusb classic connector (no matter JP2 state)
    - OR Descent 5V power (2A) via Shield DC Barrel or terminal block (JP2 on shield need to be closed)
    - OR Descent 6V/32V power (2A) Shield DC Barrel or terminal block (JP2 on shield need to be opened and DC/DC step down placed on the shield)
  - **Dual power**, one for RPI and one for ic880a (JP1 on shield need to be opened)
    - Descent 5V power (1A) via Raspberry PI microusb classic connector (no matter JP2 state) AND
    - Descent 5V power (1A) via Shield DC Barrel or terminal block (JP2 on shield need to be closed)
    - OR Descent 6V/28V power (1A) Shield DC Barrel or terminal block (JP2 on shield need to be opened and DC/DC step down placed on the shield)
- Power your RPi and wait for 1 minute
- From a computer in the same LAN, `ssh` into the RPi using the default hostname:

        local $ ssh pi@raspberrypi.local

- Default password of a plain-vanilla RASPBIAN install for user `pi` is `raspberry`
- Use `raspi-config` utility to 

        pi@raspberrypi:~ $ sudo raspi-config

  - expand the filesystem (1 Expand filesystem):
  - locale (5  Internationalisation Options / Change Locale) en us UTF8
  - timezone (5  Internationalisation Options / Change Timezone)
  - activate SPI (9 Advanced Options / SPI / Enable)
  - activate I2C (9 Advanced Options / I2C / Enable) if you put I2C sensors on shield
  - reduce video memory (9 Advanced Options / Memory Split/ Set to 16) we use SSH no desktop or monitor

- Reboot and ssh back to the device
- Make sure you have an updated installation and install `git`:

        pi@raspberrypi:~ $ sudo apt-get update
        pi@raspberrypi:~ $ sudo apt-get upgrade
        pi@raspberrypi:~ $ sudo apt-get install git-core i2c-tools

- Create new user for TTN and add it to sudoers

        pi@raspberrypi:~ $ sudo adduser ttn 

- Add TTN user creating sudoers.d/010_ttn-nopasswd file

        pi@raspberrypi:~ $ sudo visudo -f /etc/sudoers.d/010_ttn-nopasswd

Add the line `ttn ALL=(ALL) NOPASSWD: ALL`

:warning: Beware this allows a connected console with the ttn user to issue any commands on your system, without any password control. This step is completely optional and remains your decision.

- Logout and login as `ttn` and remove the default `pi` user

        $ sudo userdel -rf pi

- Configure the wifi credentials (check [here for additional details](https://www.raspberrypi.org/documentation/configuration/wireless/wireless-cli.md))

        $ sudo nano /etc/wpa_supplicant/wpa_supplicant.conf 

And add the following block at the end of the file, replacing SSID and password to match your network:

                network={
                    ssid="The_SSID_of_your_wifi"
                    psk="Your_wifi_password"
                }

Note that if you're moving gateway from one place to another during testing and thus, may have different SSID, you can add multiple access point by adding as much network as you need:

                network={
                    ssid="The_SSID_of_your_1st_access_point"
                    psk="1st_access_point_password"
                }

                network={
                    ssid="The_SSID_of_your_2nd_access_point"
                    psk="2nd_access_point_password"
                }

- Clone [the installer](https://github.com/ch2i/ic880a-gateway/) (note it's not the master branch but the ch2i-rpi-shield one) and start the installation

        $ git clone -b ch2i-rpi-shield https://github.com/ch2i/ic880a-gateway.git ~/ic880a-gateway
        $ cd ~/ic880a-gateway
        $ sudo ./install.sh


- You can test the shield board features (LED, Sensors, Wiring, ...) with some dedicated program in `src` folder

        # To check LED and make them blink (ic880a does not need to be plugged)
        $ cd ~/ic880a-gateway/src/check_led
        $ make; ./check_led

        # To check ic880a, plug it and verify
        $ cd ~/ic880a-gateway/src/check_ic880a
        $ make; ./check_ic880a

        -- Asserting CS=GPIO8 (CE0) --
        Checking SX1301 register(0x01) => SX1301 LoraWAN Concentrator (V=0x67)
        Checking SX1257 register(0x07) => Nothing!
        Checking SX1255 register(0x07) => Nothing!
        Checking RFM9x  register(0x42) => Nothing!
        Checking RFM69  register(0x10) => Nothing!


- If you added some sensors on the shield (BMP280/BME280 and/or SI7021/HTU21D) and send data to IoT dashboard (in this example Cayenne),  ~~Clone [the cayenne](https://github.com/myDevicesIoT/Cayenne-MQTT-CPP) API~~ launch script installer and type your Cayenne username, password and client ID when asked. You can get them from Cayenne dashbord (add / New Device / Bring Your Own )


```
ttn@ttn-gw02:~/ $ cd ic880a-gateway/src/sensors_js
ttn@ttn-gw02:~/ic880a-gateway/src/sensors_js $ sudo chmod ug+x install.sh
ttn@ttn-gw02:~/ic880a-gateway/src/sensors_js $ sudo ./install.sh
nodejs found, v6.9.2
bme280-sensor upgrade
si7021-sensor upgrade
cayennejs upgrade
Please enter your cayenne credentials and your
device client ID (see https://cayenne.mydevices.com/)
  Cayenne username : aaaaaaaa-bbbb-cccc-dddd-eeeeeeee
  Cayenne password : ffffffffffffffffffffffffffffffffff
  Cayenne clientID : aaaaaaaa-bbbb-cccc-dddd-eeeeeeee
Waiting service to start and connect...

Installation completed.\n
use sudo systemctl status sensors-js to see service status
use sudo journalctl -f -u sensors-js to see service log
● sensors-js.service - CH2i TTN Gateway Sensors service
   Loaded: loaded (/lib/systemd/system/sensors-js.service; enabled)
   Active: active (running) since Tue 2016-12-13 11:41:29 CET; 3s ago
 Main PID: 8295 (node)
   CGroup: /system.slice/sensors-js.service
           └─8295 /usr/bin/node /opt/ttn-gateway/bin/sensors.js

Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: BME280 data = {
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: "temperature_C": 43.54,
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: "humidity": 13.888340312121443,
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: "pressure_hPa": 1013.9026493024522
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: }
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: SI7021 initialization succeeded
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: SI7021 data = {
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: "humidity": 28.862518310546875,
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: "temperature_C": 27.496376953125
Dec 13 11:41:30 ttn-gw02 sensors-js[8295]: }
ttn@ttn-gw02:~/ic880a-gateway/src/sensors_js $
```

And it looks something like this

<img src="https://raw.githubusercontent.com/ch2i/ic880a-gateway/ch2i-rpi-shield/pictures/ch2i-gateway-monitoring-iot.jpg" alt="Ch2i LoraWan Gateway on Cayenne">


- If you want to use the remote configuration option, please make sure you have created a JSON file named as your gateway EUI (e.g. `B827EBFFFE7B80CD.json`) in the [Gateway Remote Config repository](https://github.com/ttn-zh/gateway-remote-config). 
- **Big Success!** You should now have a running gateway in front of you!


- Some tips may help you also such as Restart TTN service with
```
sudo systemctl restart ttn-gateway.service
```

- Or see log file in real time with 
```shell
sudo journalctl -f -u ttn-gateway.service

-- Logs begin at Thu 2016-08-11 21:09:47 CEST. --
Aug 12 16:16:27 pi04 ttn-gateway[1525]: ### [GPS] ###
Aug 12 16:16:27 pi04 ttn-gateway[1525]: # Invalid gps time reference (age: 1471011382 sec)
Aug 12 16:16:27 pi04 ttn-gateway[1525]: # Manual GPS coordinates: latitude 46.62477, longitude 0.43528, altitude 104 m
Aug 12 16:16:27 pi04 ttn-gateway[1525]: ##### END #####
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: End of upstream thread
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: End of downstream thread for server  0.
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: concentrator stopped successfully
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: Exiting packet forwarder program
Aug 12 16:16:27 pi04 systemd[1]: Starting The Things Network Gateway...
Aug 12 16:16:27 pi04 systemd[1]: Started The Things Network Gateway.
Aug 12 16:16:38 pi04 ttn-gateway[14558]: [TTN Gateway]: Waiting for internet connection...
Aug 12 16:17:08 pi04 ttn-gateway[14558]: *** Poly Packet Forwarder for Lora Gateway ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: Version: 2.1.0
Aug 12 16:17:08 pi04 ttn-gateway[14558]: *** Lora concentrator HAL library version info ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: Version: 3.1.0; Options: native;
Aug 12 16:17:08 pi04 ttn-gateway[14558]: ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Little endian host
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: found global configuration file global_conf.json, parsing it
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: global_conf.json does contain a JSON object named SX1301_conf, parsing SX1301 parameters
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: lorawan_public 1, clksrc 1
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Configuring TX LUT with 16 indexes
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: radio 0 enabled (type SX1257), center frequency 867500000, RSSI offset -166.000000, tx enabled 1
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: radio 1 enabled (type SX1257), center frequency 868500000, RSSI offset -166.000000, tx enabled 0
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 0>  radio 1, IF -400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 1>  radio 1, IF -200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 2>  radio 1, IF 0 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 3>  radio 0, IF -400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 4>  radio 0, IF -200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 5>  radio 0, IF 0 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 6>  radio 0, IF 200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 7>  radio 0, IF 400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora std channel> radio 1, IF -200000 Hz, 250000 Hz bw, SF 7
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: FSK channel> radio 1, IF 300000 Hz, 125000 Hz bw, 50000 bps datarate
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: global_conf.json does contain a JSON object named gateway_conf, 
```


## Specific Setup for C.H.I.P. computer

**This setup is for $9 [C.H.I.P. computer](http://docs.getchip.com/)**, you will see minor modifications done to get it working. For now, since the GPIO driving method is different, I didn't port the LED control but the gateway is fully operational.

- Flash the server image to your C.H.I.P. computer going (from Google Chrome) going to [next thing flasher](http://flash.getchip.com/) and follow the procedure to flash it
- Select when asked the Headless image (Debian server with no G.U.I)
- Since you're not connected with WIFI, first connect your C.H.I.P. with terminal using various [options](http://docs.getchip.com/chip.html#headless-chip)
- once logged with login chip and password chip, setup the [WiFi connection](http://docs.getchip.com/chip.html#wifi-connection]
- Use `nmtui` utility to change system hostname if needed

        chip@chip:~ $ sudo nmtui

- Adjust your time zone with dpkg if needed

        chip@chip:~ $ sudo dpkg-reconfigure tzdata

- Change root password 

        chip@chip:~ $ sudo passwd 

- Reboot and ssh back to the device using `ssh chip@chip.local` or any name if you changed it, mine is now chip03.local
- Make sure you have an updated installation and install all necessary software:

        chip@chip03:~ $ sudo apt-get update
        chip@chip03:~ $ sudo apt-get upgrade
        chip@chip03:~ $ sudo apt-get install build-essential git-core i2c-tools

- Create new user for TTN and add it to sudoers

        chip@chip03:~ $ sudo adduser ttn 

- Add TTN user creating sudoers.d/010_ttn-nopasswd file

        chip@chip03:~ $ sudo visudo -f /etc/sudoers.d/010_ttn-nopasswd

Add the line `ttn ALL=(ALL) NOPASSWD: ALL`

:warning: Beware this allows a connected console with the ttn user to issue any commands on your system, without any password control. This step is completely optional and remains your decision.

- Logout and login as `ttn` and remove the default `chip` user

        ttn@chip03:~ $ sudo userdel -rf chip
        ttn@chip03:~ $ sudo rm -rf /home/chip/

- Install specific tools and copy gpio tool to bin path, see how it works [there](https://bbs.nextthing.co/t/second-serial-port/4163/79)

        ttn@chip03:~ $ sudo apt-get install libusb-1.0 pkg-config
        ttn@chip03:~ $ git clone https://github.com/linux-sunxi/sunxi-tools/
        ttn@chip03:~ $ cd sunxi-tools
        ttn@chip03:~/sunxi-tools $ make ; make install

- Enable SPI at bootup adding these 2 line (before exit 0) on `/etc/rc.local` see [there](https://bbs.nextthing.co/t/second-serial-port/4163/83), then SPI #2 device will be named `/dev/spi32767.0`, check that the file is present after the next reboot.

        ttn@chip03:~ $ sudo nano /etc/rc.local

```shell
	mkdir -p /sys/kernel/config/device-tree/overlays/spi
	cat /lib/firmware/nextthingco/chip/sample-spi.dtbo > /sys/kernel/config/device-tree/overlays/spi/dtbo

	exit 0
```
- Check that the SPI device `/dev/spi32767.0` is present after the next reboot. SPI pinout is the one defined [here](http://docs.getchip.com/chip.html#pin-headers) and it's SPI #2

| PIN function | Connector Name | Header Name |
| :---: | :---: | :---: |
|	CS	 | U14_27 | CSIPCK |
|	CLK	 | U14_28 | CSICK |
| MOSI | U14_29 | CSIHSYNC |
|	MISO | U14_30 | CSIVSYNC |

The iC880A extented connection pin are the following, I've put the mapping of sunxi-pio tool, the tool to drive I/O pins

| PIN function | Connector Name | Header Name | sunxi-pio Name |
| :---: | :---: | :---: | :---: |
|	iC880AReset | U14_31 | CSID0 | PE4 |
|	Led Heartbeat | U14_35 | CSID4 | PE8 |
| Led Down | U14_34 | CSID3 | PE7 |
|	Led RX Er | U14_37 | CSID6 | PE10 |
|	Led RX OK | U14_32 | CSID1 | PE5 |


- Clean shutdown the C.H.I.P. and wait LED are off 

        ttn@chip03:~ $ sudo shutdown -h now

- Plug the iC880a on the shield then the shield on the C.H.I.P. computer (not available yet, use dupond wires for now)
- wire power to the whole thing using using this following option. Since CHIP is not able to provide sufficent power to iC880A from its power engine and there is no 5V on the connectors issue from main 5V USB line, we need to use external power 5V or more so you can choose berween
    - Descent 5V power (2A) via Shield DC Barrel or terminal block (JP2 on shield need to be closed)
    - OR Descent 6V/32V power (2A) Shield DC Barrel or terminal block (JP2 on shield need to be opened and DC/DC step down placed on the shield)
- You need to connect power from PI shield to C.H.I.P. [connector U13](http://docs.getchip.com/chip.html#pin-headers) pin 1 (GND) and pin 2 (CHG-IN). Do not use other 5V pin of CHIP, they are just output from CHIP internal DC/DC converter. You can take both GND and 5V signal from anywhere on RPI Shield. I've done for now with PI connector named P1 pin 2 (5V) and 6 (GND)
- Power the Shield and wait for 1 minute C.H.I.P. to boot


- For now, clone [the default installer](https://github.com/ttn-zh/ic880a-gateway/tree/spi) (note it's not the master branch but the spi one) and start the installation


        $ git clone -b spi https://github.com/ttn-zh/ic880a-gateway ~/ic880a-gateway
        $ cd ~/ic880a-gateway

- change the sartup script `start.sh` to add the concentrator reset at the begining, script should looks like this


        ttn@chip03:~ $ sudo nano /opt/ttn-gateway/bin/start.sh

				```shell
				#! /bin/bash

				# Reset iC880a PIN (PE4=CSID0)
				SX1301_RESET_PIN=PE4

				/usr/local/bin/sunxi-pio -m "$SX1301_RESET_PIN<1><0><1><0>"
				sleep 0.1
				/usr/local/bin/sunxi-pio -m $SX1301_RESET_PIN=1
				sleep 0.1
				/usr/local/bin/sunxi-pio -m $SX1301_RESET_PIN=0

				# Test the connection, wait if needed.
				# ...
				```


        $ sudo ./install.sh

- The library libloragw has been compiled for Raspberry PI so it won't works until you decide to make some changes for C.H.I.P., one of this
  - This one has not been tested, just try it and tell me. Create a symlink `ln -sf /dev/spi32767.0 /dev/spidev0.0` so spi device has the same name than on RPI
  - If this does not works, just create new `imst_chip.h` file copied from original `imst_rpi.h`


        $ cd /opt/ttn-gateway/lora_gateway/libloragw/
        $ cp inc/imst_rpi.h inc/imst_chip.h

  - then change the file `inc/imst_chip.h` to the following

		```cpp
		/*
		* imst_chip.h
		*
		*  Created on: Dec 08, 2016
		*      Author: Charles-Henri Hallard
		*/

		#ifndef _IMST_CHIP_H_
		#define _IMST_CHIP_H_

		/* Human readable platform definition */
		#define DISPLAY_PLATFORM "IMST + C.H.I.P."

		/* parameters for native spi */
		#define SPI_SPEED   8000000
		#define SPI_DEV_PATH  "/dev/spi32767.0"
		#define SPI_CS_CHANGE   0

		/* parameters for a FT2232H */
		#define VID           0x0403
		#define PID           0x6014

		#endif /* _IMST_CHIP_H_ */
		```

  - Now change the library configuration file `library.cfg` to set platform on the dedicated section, the line PLATFORM should now be

		```
		### Specify which platform you are on.
		# Accepted values:
		#  kerlink     This is the default from Semtech and works for the Kerlink
		#  lorank      This is for the Lorank (BeagleBone + IMST concentrator).
		#  imst_rpi    This is for the IMST concentrators with a Raspberry Pi host.
		#  imst_chip   This is for the IMST concentrators with a C.H.I.P. host.
		#  linklabs_blowfish_rpi This is for the LinkLabs concentrators with a Raspberry Pi host.

		PLATFORM= imst_chip
		```

  - Rebuild the libloragw library


        # from /opt/ttn-gateway/lora_gateway/libloragw
        $ sudo make uninstall
        $ make 
        $ sudo make install
        $ sudo ldconfig
        

- You can test the concentrator presence with compiled `test_loragw_reg` after reset of the concentrator


        # from /opt/ttn-gateway/lora_gateway/libloragw
        $ sudo sunxi-pio -m "PE4<1><0><1><0>"
        $ sudo sunxi-pio -m PE4=1
        $ sudo sunxi-pio -m PE4=0
        $ sudo ./test_loragw_reg

- End of output should be like this (should be same numbers)

```
End of register verification
IMPLICIT_PAYLOAD_LENGHT = 197 (should be 197)
FRAME_SYNCH_PEAK2_POS = 11 (should be 11)
PREAMBLE_SYMB1_NB = 49253 (should be 49253)
ADJUST_MODEM_START_OFFSET_SF12_RDX4 = 3173 (should be 3173)
IF_FREQ_1 = -1947 (should be -1947)
End of test for loragw_reg.c
```

- If you want to use the remote configuration option, please make sure you have created a JSON file named as your gateway EUI (e.g. `B827EBFFFE7B80CD.json`) in the [Gateway Remote Config repository](https://github.com/ttn-zh/gateway-remote-config). 
- **Big Success!** You should now have a running gateway in front of you!


- Some tips may help you also such as Restart TTN service with
```
sudo systemctl restart ttn-gateway.service
```

- Or see log file in real time with 
```shell
sudo journalctl -f -u ttn-gateway.service

-- Logs begin at Thu 2016-08-11 21:09:47 CEST. --
Aug 12 16:16:27 pi04 ttn-gateway[1525]: ### [GPS] ###
Aug 12 16:16:27 pi04 ttn-gateway[1525]: # Invalid gps time reference (age: 1471011382 sec)
Aug 12 16:16:27 pi04 ttn-gateway[1525]: # Manual GPS coordinates: latitude 46.62477, longitude 0.43528, altitude 104 m
Aug 12 16:16:27 pi04 ttn-gateway[1525]: ##### END #####
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: End of upstream thread
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: End of downstream thread for server  0.
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: concentrator stopped successfully
Aug 12 16:16:27 pi04 ttn-gateway[1525]: INFO: Exiting packet forwarder program
Aug 12 16:16:27 pi04 systemd[1]: Starting The Things Network Gateway...
Aug 12 16:16:27 pi04 systemd[1]: Started The Things Network Gateway.
Aug 12 16:16:38 pi04 ttn-gateway[14558]: [TTN Gateway]: Waiting for internet connection...
Aug 12 16:17:08 pi04 ttn-gateway[14558]: *** Poly Packet Forwarder for Lora Gateway ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: Version: 2.1.0
Aug 12 16:17:08 pi04 ttn-gateway[14558]: *** Lora concentrator HAL library version info ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: Version: 3.1.0; Options: native;
Aug 12 16:17:08 pi04 ttn-gateway[14558]: ***
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Little endian host
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: found global configuration file global_conf.json, parsing it
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: global_conf.json does contain a JSON object named SX1301_conf, parsing SX1301 parameters
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: lorawan_public 1, clksrc 1
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Configuring TX LUT with 16 indexes
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: radio 0 enabled (type SX1257), center frequency 867500000, RSSI offset -166.000000, tx enabled 1
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: radio 1 enabled (type SX1257), center frequency 868500000, RSSI offset -166.000000, tx enabled 0
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 0>  radio 1, IF -400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 1>  radio 1, IF -200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 2>  radio 1, IF 0 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 3>  radio 0, IF -400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 4>  radio 0, IF -200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 5>  radio 0, IF 0 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 6>  radio 0, IF 200000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora multi-SF channel 7>  radio 0, IF 400000 Hz, 125 kHz bw, SF 7 to 12
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: Lora std channel> radio 1, IF -200000 Hz, 250000 Hz bw, SF 7
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: FSK channel> radio 1, IF 300000 Hz, 125000 Hz bw, 50000 bps datarate
Aug 12 16:17:08 pi04 ttn-gateway[14558]: INFO: global_conf.json does contain a JSON object named gateway_conf, 
```

# Credits

These scripts are largely based on the awesome work by [Ruud Vlaming](https://github.com/devlaam) on the [Lorank8 installer](https://github.com/Ideetron/Lorank)
They have been adapted [Gonzalo Casas](https://github.com/gonzalocasas) and [galagaking](https://github.com/galagaking) and adapted to works on this Raspberry PI shield by Charles-Henri Hallard from [CH2i](https://github.com/ch2i).