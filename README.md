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


- If you added some sensors on the shield (BME280 and SI7021/HTU21D) and send data to IoT dashboard (in this example Cayenne),  Clone [the cayenne](https://github.com/myDevicesIoT/Cayenne-MQTT-CPP) API. 


        # To check sensors if any (BME280 and/or SI7021 or HTU21D)
        $ cd ~/ic880a-gateway
        $ git clone https://github.com/myDevicesIoT/Cayenne-MQTT-CPP
        $ cd ic880a-gateway/src/sensors


Don't forget to put your Cayenne username, password and client ID. You get the from Cayenne dashbord (add / New Device / Bring Your Own Thing) and report then back in file sensors.cpp before compilation
```cpp
// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "a1ced9e0-b24e-11e6-bb76-1157AA55AA55";
char password[] = "0858f39268653283bf68bb08b165c07cAA55AA55";
char clientID[] = "bd6c2ab0-bd1e-11e6-9638-53ecAA55AA55";
```

        $ make; ./sensors
        Checking BMP280 or BME280 device...BME280, OK!
        Checking SI7021 or HTU21D device...SI7021 found
        Connecting to mqtt.mydevices.com:1883
        Connected
        BME280 reading
          51.99 C
          12.52 %RH
          1019.37 hPa
          -50.82 m
        SI7021 Reading
          Temp : 36.13C
          Hum : 19.1%rh
        ^C
        Break received, exiting!

And it looks something like this

<img src="https://raw.githubusercontent.com/ch2i/ic880a-gateway/ch2i-rpi-shield/pictures/ch2i-gateway-monitoring-iot.jpg" alt="Ch2i LoraWan Gateway on Cayenne">


- If you want to use the remote configuration option, please make sure you have created a JSON file named as your gateway EUI (e.g. `B827EBFFFE7B80CD.json`) in the [Gateway Remote Config repository](https://github.com/ttn-zh/gateway-remote-config). 
- **Big Success!** You should now have a running gateway in front of you!

# Credits

These scripts are largely based on the awesome work by [Ruud Vlaming](https://github.com/devlaam) on the [Lorank8 installer](https://github.com/Ideetron/Lorank)
They have been adapted [Gonzalo Casas](https://github.com/gonzalocasas) and [galagaking](https://github.com/galagaking) and adapted to works on this Raspberry PI shield by Charles-Henri Hallard from [CH2i](https://github.com/ch2i).