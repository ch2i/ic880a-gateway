# Makefile
# Sample for spi check_led on Raspberry Pi
# Caution: requires bcm2835 library to be already installed
# http://www.airspayce.com/mikem/bcm2835/

CC            = g++
CFLAGS        = -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\"
LIBS          = -lbcm2835 

all: check_led

check_led.o: check_led.c
				$(CC) $(CFLAGS) -c $<

check_led: check_led.o
				$(CC) $^ $(LIBS) -o check_led

clean:
				rm -rf *.o check_led
