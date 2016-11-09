// check_led.cpp
//
// Example program showing how to check LED on Raspberry PI ic880a shield
// Requires bcm2835 library to be already installed
// http://www.airspayce.com/mikem/bcm2835/
// Use the Makefile in this directory:
// cd src/check_led
// make
// sudo ./check_led
//
// Will blink LED on GPIO4, GPIO18, GPIO23 and GPIO24
// RasPI Lora Gateway Board iC880A 
// https://github.com/ch2i/iC880A-Raspberry-PI
//
// Contributed by Charles-Henri Hallard (hallard.me)
//

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>    

// Blink delay (in ms)
#define BLINK_DELAY 100

int     exit_sig   = 0; // application terminates cleanly
uint8_t LED_pins[] = {4, 18, 23, 24}; // GPIO line LED

void sig_handler(int sig) {
    if (sig==SIGQUIT || sig==SIGINT || sig==SIGTERM) {
        exit_sig = 1;
    }
}

int main(int argc, char **argv)
{
  uint8_t i;

  struct sigaction sigact; 

  printf("%s starting...\n", __BASEFILE__);

	
  // configure signal handling 
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  sigact.sa_handler = sig_handler;
  sigaction(SIGQUIT, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);
  sigaction(SIGTERM, &sigact, NULL);

  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root??\n");

  } else {

    // Drive all LED line as output and set them OFF
    for ( i=0; i<sizeof(LED_pins); i++) {
      bcm2835_gpio_fsel (LED_pins[i], BCM2835_GPIO_FSEL_OUTP );
      bcm2835_gpio_write(LED_pins[i], HIGH );
    }

    // Loop until end
    while (!exit_sig) {
      // Now try to detect all modules
      for ( i=0; i<sizeof(LED_pins); i++) {
        // Light ON
        bcm2835_gpio_write(LED_pins[i], LOW);
				if (exit_sig)
					break;
        bcm2835_delay(BLINK_DELAY*4);
        // Light OFF
        bcm2835_gpio_write(LED_pins[i], HIGH);
				if (exit_sig)
					break;
        bcm2835_delay(BLINK_DELAY);
      }
    }
  }

  printf("Received ending signal, exiting...\n");

  // all LED OFF
  for ( i=0; i<sizeof(LED_pins); i++) {
    bcm2835_gpio_write(LED_pins[i], LOW );
  }

  bcm2835_close();
}


