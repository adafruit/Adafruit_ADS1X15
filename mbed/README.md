Adafruit_ADS1015 & ADS1115 for mbed
===================================

Use like this:

```
#include "mbed.h"
#include "Adafruit_ADS1015.h"
#include "USBSerial.h"

#define SERIAL_BAUD_RATE    9600

I2C i2c(p23, p18); // Wifi DipCortex from SolderSplashlab (NXP ARM Cortex M3 LPC1347)
Adafruit_ADS1015 ads(&i2c); // send pointer to i2c, address optional (default 0x48)
USBSerial pc; // USB CDC serial port

 
int main() {
    uint16_t reading;
    while (1) {
        reading = ads.readADC_SingleEnded(0); // read channel 0
        pc.printf("reading channel 0: %d\r\n", reading); // print result to USB CDC
        wait(1); // don't loop like a crazy man
    }
}
```
