#!/bin/bash

# Stop on the first sign of trouble
set -e

if [ $UID != 0 ]; then
    echo "ERROR: Operation not permitted. Forgot sudo?"
    exit 1
fi

# Install repositories
INSTALL_DIR="/opt/ttn-gateway"
if [ ! -d "$INSTALL_DIR" ]; then mkdir $INSTALL_DIR; fi
if [ ! -d "$INSTALL_DIR/bin" ]; then mkdir $INSTALL_DIR/bin; fi

# Check dependencies
echo "Installing NodeJS dependencies libraries..."
npm install bme280-sensor si7021-sensor cayennejs
if [ ! -d node_modules/bme280-sensor ]; then npm install bme280-sensor; fi
if [ ! -d node_modules/si7021-sensor ]; then npm install si7021-sensor; fi
if [ ! -d node_modules/cayennejs ]; then npm install cayennejs; fi

# cp nodejs sensor script
cp -f ./sensors.js $INSTALL_DIR/bin/
cp -f -R ./node_modules $INSTALL_DIR/bin/

# cp startup script as a service
cp ./sensors-js.service /lib/systemd/system/

# Enable the service
systemctl enable sensors-js.service
systemctl start sensors-js.service

# wait service to read values
sleep 1

echo
echo "Installation completed.\n"
echo "use sudo systemctl status sensors-js to see service status"
echo "use sudo journalctl -f -u sensors-js to see service log"
echo -n
systemctl status sensors-js.service


