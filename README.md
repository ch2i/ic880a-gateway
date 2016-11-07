# The Things Network: iC880a-based gateway

Reference setup for [The Things Network](http://thethingsnetwork.org/) gateways based on the iC880a USB concentrator with a Raspberry Pi host.

This installer targets the **[CH2i SPI Raspberry PI shield version](https://github.com/ch2i/iC880A-Raspberry-PI)** of the board.
<img src="https://raw.githubusercontent.com/ch2i/ic880a-gateway/master/pictures/ch2i-shield-mounted.jpg" alt="Full stack" width="25%" height="25%"> 

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
    - OR Descent 6V/32V power (1A) Shield DC Barrel or terminal block (JP2 on shield need to be opened and DC/DC step down placed on the shield)
- Power your RPi and wait for 1 minute
- From a computer in the same LAN, `ssh` into the RPi using the default hostname:

        local $ ssh pi@raspberrypi.local

- Default password of a plain-vanilla RASPBIAN install for user `pi` is `raspberry`
- Use `raspi-config` utility to 

        pi@raspberrypi:~ $ sudo raspi-config

  - expand the filesystem (1 Expand filesystem):
  - locale (5  Internationalisation Options / Change Locale)
  - timezone (5  Internationalisation Options / Change Timezone)
  - activate SPI (9 Advanced Options / SPI / Enable)
  - reduce video memory (9 Advanced Options / Memory Split/ Set to 16) we use SSH no desktop or monitor

- Reboot and ssh back to the device
- Make sure you have an updated installation and install `git`:

        pi@raspberrypi:~ $ sudo apt-get update
        pi@raspberrypi:~ $ sudo apt-get upgrade
        pi@raspberrypi:~ $ sudo apt-get install git-core

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
 
- Clone [the installer](https://github.com/ch2i/ic880a-gateway/) and start the installation

        $ git clone -b ch2i-rpi-shield https://github.com/ch2i/ic880a-gateway.git ~/ic880a-gateway
        $ cd ~/ic880a-gateway
        $ sudo ./install.sh

- If you want to use the remote configuration option, please make sure you have created a JSON file named as your gateway EUI (e.g. `B827EBFFFE7B80CD.json`) in the [Gateway Remote Config repository](https://github.com/ttn-zh/gateway-remote-config). 
- **Big Success!** You should now have a running gateway in front of you!

# Credits

These scripts are largely based on the awesome work by [Ruud Vlaming](https://github.com/devlaam) on the [Lorank8 installer](https://github.com/Ideetron/Lorank)
They have been adapted [Gonzalo Casas](https://github.com/gonzalocasas) and [galagaking](https://github.com/galagaking) and adapted to works on this Raspberry PI shield by Charles-Henri Hallard from [CH2i](https://github.com/ch2i).