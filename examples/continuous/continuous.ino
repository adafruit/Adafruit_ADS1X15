#include <Wire.h>
#include <Adafruit_ADS1015.h>

//Adafruit_ADS1115 ads;     // Use this for the 16-bit version 
Adafruit_ADS1015 ads;     // Use this for the 12-bit version 

const int alertPin = 2;

volatile bool continuousConversionReady = false;
  
void setup(void)
{
  pinMode(alertPin,INPUT);

  Serial.begin(9600);
  Serial.println("Hello!");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
     ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  //ads.begin(1,0);                // for ESP8266  SDA, SCL can be specified
  ads.begin();
  
  
  Serial.println("Starting continous mode on A0 at 8 SPS");
  ads.setSPS(ADS1115_DR_8SPS);     
  ads.startContinuous_SingleEnded(0); 
  //ads.startContinuous_SingleEnded(1); 
  //ads.startContinuous_SingleEnded(2); 
  //ads.startContinuous_SingleEnded(3); 
  //ads.startContinuous_Differential_0_1(); 
  //ads.startContinuous_Differential_0_3(); 
  //ads.startContinuous_Differential_1_3(); 
  //ads.startContinuous_Differential_2_3(); 
  
  attachInterrupt(digitalPinToInterrupt(alertPin), continuousAlert, FALLING);
}

void continuousAlert() {

// Do not call getLastConversionResults from ISR because it uses I2C library that needs interrupts
// to make it work, interrupts would need to be re-enabled in the ISR, which is not a very good practice.
  
  continuousConversionReady = true;
}

void loop(void)
{
  if (continuousConversionReady) {    
    float result = ((float) ads.getLastConversionResults()) * ads.voltsPerBit();
    continuousConversionReady = false;
    Serial.print ("In interrupt routine. Reading is ");
    Serial.print (result,7);
    Serial.print (" at millisecond ");
    Serial.println(millis());
  }

}