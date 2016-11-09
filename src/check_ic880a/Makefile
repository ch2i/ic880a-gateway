# Makefile
# Sample for spi scan SX1301 concentrator board on Raspberry Pi
# Caution: requires bcm2835 library to be already installed
# http://www.airspayce.com/mikem/bcm2835/

CC            = g++
CFLAGS        = -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\"
LIBS          = -lbcm2835 

all: check_ic880a

check_ic880a.o: check_ic880a.c
				$(CC) $(CFLAGS) -c $<

check_ic880a: check_ic880a.o
				$(CC) $^ $(LIBS) -o check_ic880a

clean:
				rm -rf *.o check_ic880a
