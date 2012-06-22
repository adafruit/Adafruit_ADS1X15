#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1015 ads1015;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  ads1015.begin();
}

void loop(void) 
{
  int16_t results;

  results = ads1015.readADC_Differential_0_1();  
  Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(results * 3); Serial.println("mV)");

  delay(1000);
}