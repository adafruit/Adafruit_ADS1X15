/*
This sketch tests the library, outputting the values ready to Serial.
Only one gain setting is tested at a time.  If the gain is changed to
GAIN_SIXTEEN then some of the read voltagees exceed the gain range and should
read as the max value.  With the resistors below all other gains should 
produce correct results (within a reasonable tolerance). Note that reading
A0 (GND) may produce a slightly negative voltage

Sketch and circuit by soligen2010, Feb 12, 2016

Below are the components and connections for the test circuit for the test:

Components:

ADS1115
D1 1N4004 (Cathode to AIN0, Anode to Pin 3 for arduino - choose different pin for ESP8266)
R1 3k3    (VDD to AIN0)
R2 240R   (AIN0 to AIN1)
R3 1K     (AIN1 to AIN2)
R4 1K     (AIN2 to AIN3)

Connections:

VDD ---- +5, R1 (VDD to AIN0)
GND ---- Gound
SCL ---- Arduino Uno A5 or ESP8266 Pin 5
SDA ---- Arduino Uno A4 or ESP8266 Pin 4
ADDR --- GND
ALRT --- Digital Pin 2
AIN0 ----- R1, D1(Cathode to AIN0, Anode to Pin 3), R2
AIN1 ----- R2, R3
AIN2 ----- R3, R4
AIN3 ----- R4, GND

*/

#include <Wire.h>
#include <Adafruit_ADS1015.h>

//Adafruit_ADS1115 ads;     // Use this for the 16-bit version 
Adafruit_ADS1015 ads;     // Use this for the 12-bit version 

float   multiplier;

const int alertPin = 2;
const int raiseVoltagePin = 3;

volatile bool continuousConversionReady = false;
  
void setup(void)
{
  pinMode(alertPin,INPUT);
  pinMode(raiseVoltagePin,OUTPUT);
  digitalWrite(raiseVoltagePin,LOW);
  
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
  
  multiplier = ads.voltsPerBit()*1000.0F;           // Gets the millivolts per bit 
  
  /* Use this to set data rate for the 12-bit version (optional)*/
  //ads.setSPS(ADS1015_DR_3300SPS);                 // for ADS1015 fastest samples per second is 3300 (default is 1600)
  
  /* Use this to set data rate for the 16-bit version (optional)*/
  ads.setSPS(ADS1115_DR_8SPS);                      // for ADS1115 fastest samples per second is 860 (default is 128)

  ads.readADC_Differential_0_1();                   // in case chip was previously in contuous mode, take out of continuous
  ads.waitForConversion();                          // delay to ensure any last remaining conversion completes
                                                    // needed becasue if formerly was in continuous, 2 conversions need to complete
  
  // Run test for each SPS
  
  Serial.println("********* 8 SPS ************");
  ads.setSPS(ADS1115_DR_8SPS);     
  runTest();
  
  Serial.println("********* 16 SPS ************");
  ads.setSPS(ADS1115_DR_16SPS);     
  runTest();
  
  Serial.println("********* 32 SPS ************");
  ads.setSPS(ADS1115_DR_32SPS);     
  runTest();
  
  Serial.println("********* 64 SPS ************");
  ads.setSPS(ADS1115_DR_64SPS);     
  runTest();
  
  Serial.println("********* 128 SPS ************");
  ads.setSPS(ADS1115_DR_128SPS);     
  runTest();
  
  Serial.println("********* 250 SPS ************");
  ads.setSPS(ADS1115_DR_250SPS);     
  runTest();
  
  Serial.println("********* 475 SPS ************");
  ads.setSPS(ADS1115_DR_475SPS);     
  runTest();
  
  Serial.println("********* 860 SPS ************");
  ads.setSPS(ADS1115_DR_860SPS);     
  runTest();

//Configure interrupt on alert pin for continuous mode;
  Serial.println();
  
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
  
  pinMode(alertPin, INPUT);
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

void runTest() 
{
  int16_t results0_1, results0_3, results1_3, results2_3;   

  results0_1 = ads.readADC_Differential_0_1();
  Serial.print("Differential 0-1: "); Serial.print(results0_1); Serial.print("("); Serial.print(results0_1 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_Differential_0_1_V());

  results0_3 = ads.readADC_Differential_0_3();
  Serial.print("Differential 0-3: "); Serial.print(results0_3); Serial.print("("); Serial.print(results0_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_Differential_0_3_V());

  results1_3 = ads.readADC_Differential_1_3();
  Serial.print("Differential 1-3: "); Serial.print(results1_3); Serial.print("("); Serial.print(results1_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_Differential_1_3_V());

  results2_3 = ads.readADC_Differential_2_3();
  Serial.print("Differential 2-3: "); Serial.print(results2_3); Serial.print("("); Serial.print(results2_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_Differential_2_3_V());

  results0_1 = ads.readADC_SingleEnded(0);
  Serial.print("Single 0: "); Serial.print(results0_1); Serial.print("("); Serial.print(results0_1 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_SingleEnded_V(0));

  results0_3 = ads.readADC_SingleEnded(1);
  Serial.print("Single 1: "); Serial.print(results0_3); Serial.print("("); Serial.print(results0_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_SingleEnded_V(1));

  results1_3 = ads.readADC_SingleEnded(2);
  Serial.print("Single 2: "); Serial.print(results1_3); Serial.print("("); Serial.print(results1_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_SingleEnded_V(2));

  results2_3 = ads.readADC_SingleEnded(3);
  Serial.print("Single 3: "); Serial.print(results2_3); Serial.print("("); Serial.print(results2_3 * multiplier); Serial.println("mV)");
  Serial.println(1000.0F*ads.readADC_SingleEnded_V(3));

  Serial.print("Read Time Microseconds: ");
  uint32_t sampleTime = micros();
  results2_3 = ads.readADC_SingleEnded(3);
  Serial.println(micros() - sampleTime);

  // Test comparator mode
  
  int16_t compThresh = ads.readADC_SingleEnded(0) + 200;       // Set the threshold slighly above A0
 
  ads.startComparator_SingleEnded(0,compThresh);
  Serial.print("Comparator started.  Alert Pin (should be 1) = ");Serial.println(digitalRead(alertPin));
  digitalWrite(raiseVoltagePin,HIGH);                          // raise the voltage.  This feeds 5V via D1 into A0, bypassing R1
  do {delay(0);} while (digitalRead(alertPin) == HIGH);        // Wait until alert signaled
  Serial.print("Comparator triggered.  Alert Pin (should be 0) = ");Serial.println(digitalRead(alertPin));
  digitalWrite(raiseVoltagePin,LOW);                           // lower voltage back down
  delay(1);
  results0_3 = ads.getLastConversionResults();                 // reading conversion clears the latch
  results0_1 = ads.readADC_SingleEnded(0);                     // stops the contuous reading of the comparator mode
  ads.waitForConversion();                                     // delay to be sure last conversion ends. changing
                                                               // from continuous to single the read may pick up the last continuous
                                                               // instead of the newly initiated single read              
  
  Serial.println();

  delay(2000);
}
