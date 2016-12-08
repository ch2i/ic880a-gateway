# Makefile
# Sample for ic880a Lorawan Shield for Raspberry PI with sensors
# Caution: requires bcm2835 library to be already installed
# http://www.airspayce.com/mikem/bcm2835/
# Requires Cayenne MQTT CPP API
# https://github.com/myDevicesIoT/Cayenne-MQTT-CPP

SRC_DIR := ../../../Cayenne-MQTT-CPP/src
PLATFORM_DIR := $(SRC_DIR)/Platform/Linux
EXAMPLES_DIR := $(PLATFORM_DIR)/examples
BUILD_DIR := build
CC := gcc
COMMON_INCLUDE_DIRS := -I$(SRC_DIR)/MQTTCommon -I$(PLATFORM_DIR)
CFLAGS := -Wall -Wstrict-prototypes -O2 -MMD -MP $(COMMON_INCLUDE_DIRS)
CXXFLAGS := -Wall -O2 -MMD -MP $(COMMON_INCLUDE_DIRS) -I$(SRC_DIR)/CayenneMQTTClient 
CSENSORSFLAGS := -D__BASEFILE__=\"$*\" -DBCM2835_NO_DELAY_COMPATIBILITY

#Paths containing source files
vpath %c $(SRC_DIR)/CayenneUtils:$(SRC_DIR)/MQTTCommon:$(EXAMPLES_DIR)
vpath %cpp $(SRC_DIR)/CayenneMQTTClient:$(SRC_DIR)/CayenneUtils:$(SRC_DIR)/MQTTCommon:$(EXAMPLES_DIR)

COMMON_SOURCES := $(notdir $(SRC_DIR)/CayenneUtils/CayenneUtils.c $(wildcard $(SRC_DIR)/MQTTCommon/*.c))
COMMON_OBJS := $(COMMON_SOURCES:.c=.o)
NETWORK_OBJS := Network.o

#Ojbects and dependency files for cayenne
CAYENNE_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) sensors.o)
#Ojbects and dependency files for sensors
SENSORS_OBJS := $(addprefix $(BUILD_DIR)/, $(COMMON_OBJS) i2c.o si7021.o bme280.o)

.PHONY: all sensors clean

all: sensors

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<	

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(CSENSORSFLAGS) -c -o $@ $<

	#build
sensors: $(CAYENNE_OBJS) $(SENSORS_OBJS)
	$(CC) $(CPPFLAGS) $^ -o $@ -lbcm2835 -lm
	
clean:
	rm -r -f $(BUILD_DIR)
	rm -f sensors

-include $(BUILD_DIR)/*.d 
