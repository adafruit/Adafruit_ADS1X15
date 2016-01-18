#include <Wire.h>
#include <Adafruit_ADS1015.h>

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

float   multiplier;
  
void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
  
  multiplier = ads.voltsPerBit()*1000.0F;    /* Sets the millivolts per bit */
  
  /* Use this to set data rate for the 12-bit version (optional)*/
  ads.setSPS(ADS1015_DR_3300SPS);      // for ADS1015 fastest samples per second is 3300 (default is 1600)
  
  /* Use this to set data rate for the 16-bit version (optional)*/
  //ads.setSPS(ADS1115_DR_860SPS);      // for ADS1115 fastest samples per second is 860 (default is 128)

}

void loop(void)
{
  int16_t results0_1, results0_3, results1_3, results2_3;
   

  results0_1 = ads.readADC_Differential_0_1();
  Serial.print("Differential 0-1: "); Serial.print(results0_1); Serial.print("("); Serial.print(results0_1 * multiplier); Serial.println("mV)");

  results0_3 = ads.readADC_Differential_0_3();
  Serial.print("Differential 0-3: "); Serial.print(results0_3); Serial.print("("); Serial.print(results0_3 * multiplier); Serial.println("mV)");

  results1_3 = ads.readADC_Differential_1_3();
  Serial.print("Differential 1-3: "); Serial.print(results1_3); Serial.print("("); Serial.print(results1_3 * multiplier); Serial.println("mV)");

  results2_3 = ads.readADC_Differential_2_3();
  Serial.print("Differential 2-3: "); Serial.print(results2_3); Serial.print("("); Serial.print(results2_3 * multiplier); Serial.println("mV)");


  delay(1000);
}
