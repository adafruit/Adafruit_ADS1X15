/**************************************************************************/
/*!
    @file     Adafruit_ADS1015.cpp
    @author   K.Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    Driver for the ADS1015/ADS1115 ADC

    This is a library for the Adafruit MPL115A2 breakout
    ----> https://www.adafruit.com/products/???

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
    v1.1 - Added ADS1115 support - W. Earl
    v1.2 - Modified by soligen2010 - consolidation of pull requests and other stuff
	       Moved conversion delay out of getLastConversionResults into startComparator_SingleEnded so the method can be used more generically.
           Re-factored out duplicated code in differential reads
		   Re-factored out duplicated code for single ended MUX bit settings
           Added support for differential reads between pins 0,3 and 1,3 (was also done by ycheneval in pull request #6)
           Added voltsPerBit() as a convenience
		   Added keywords.txt (Pull request #8 from dhhagen)
		   Fix for single ended int overflow (Pull request #9 from jamesfowkes)
		   Added include guard (Pull request #11 from Ivan-Perez)
		   Added begin method for ESP8266 systems (from Pontus Oldberg)
		   Ability to configure Samples per Second and set corresponding Conversion Delay (Adapted from romainreignier)
		   In startComparator_SingleEnded explicitly set the low threshold to the default.
		   Added methods that return actual voltage as a float.
		   Renamed constants that were common to both chips to ADS1X15....
	v1.2.1 Modified by soligen2010. Removed explicit conversion delays and instead poll the config
	       register by 1 ms intervals to check when conversion is complete
		   
*/
/**************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include "Adafruit_ADS1015.h"

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) {
  Wire.beginTransmission(i2cAddress);
  i2cwrite((uint8_t)reg);
  i2cwrite((uint8_t)(value>>8));
  i2cwrite((uint8_t)(value & 0xFF));
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits to the specified destination register
*/
/**************************************************************************/
static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) {
  Wire.beginTransmission(i2cAddress);
  i2cwrite(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, (uint8_t)2);
  return ((i2cread() << 8) | i2cread());  
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1015 class w/appropriate properties
*/
/**************************************************************************/
Adafruit_ADS1015::Adafruit_ADS1015(uint8_t i2cAddress) 
{
   m_i2cAddress = i2cAddress;
   m_bitShift = ADS1015_CONV_REG_BIT_SHIFT_4;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1115 class w/appropriate properties
*/
/**************************************************************************/
Adafruit_ADS1115::Adafruit_ADS1115(uint8_t i2cAddress)
{
   m_i2cAddress = i2cAddress;
   m_bitShift = ADS1115_CONV_REG_BIT_SHIFT_0;
}

/**************************************************************************/
/*!
    @brief  Sets up the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
void Adafruit_ADS1015::begin() {
  Wire.begin();
}

#if defined(ARDUINO_ARCH_ESP8266)
/**************************************************************************/
/*!
    @brief  Sets up the HW (reads coefficients values, etc.)
            This function should be called if you are using an ESP8266 and
            have the SDA and SCL pins other than 4 and 5.
*/
/**************************************************************************/
void Adafruit_ADS1015::begin(uint8_t sda, uint8_t scl) {
  Wire.begin(sda, scl);
}
#endif

/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range
*/
/**************************************************************************/
void Adafruit_ADS1015::setGain(adsGain_t gain)
{
  m_gain = gain;
}

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range
*/
/**************************************************************************/
adsGain_t Adafruit_ADS1015::getGain()
{
  return m_gain;
}

/**************************************************************************/
/*!
    @brief  Sets the Samples per Second setting
*/
/**************************************************************************/
void Adafruit_ADS1015::setSPS(adsSPS_t SPS)
{
  m_SPS = SPS;
}

/**************************************************************************/
/*!
    @brief  Gets the Samples per Second setting
*/
/**************************************************************************/
adsSPS_t Adafruit_ADS1015::getSPS()
{
  return m_SPS;
}

/**************************************************************************/
/*!
    @brief  Given the input pin (channel) Determines the MUX bits in the config 
	        register for single ended operations
*/
/**************************************************************************/
uint16_t getSingleEndedConfigBitsForMUX(uint8_t channel) {
  uint16_t c = 0;
  switch (channel)
  {
    case (0):
      c = ADS1X15_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      c = ADS1X15_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      c = ADS1X15_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      c = ADS1X15_REG_CONFIG_MUX_SINGLE_3;
      break;
  }
  return c;
}

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel in Volts
*/
/**************************************************************************/
float Adafruit_ADS1015::readADC_SingleEnded_V(uint8_t channel) {
	return (float)readADC_SingleEnded(channel) * voltsPerBit();
}

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_SingleEnded(uint8_t channel) {
  if (channel > 3)
  {
    return 0;
  }
  
  // Start with default values
  uint16_t config = ADS1X15_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1X15_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;
  
  // Set Samples per Second
  config |= m_SPS;

  // Set single-ended input channel
  config |= getSingleEndedConfigBitsForMUX(channel);

  // Set 'start single-conversion' bit
  config |= ADS1X15_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1X15_REG_POINTER_CONFIG, config);
  
  // Wait for the conversion to complete
  waitForConversion();

  return getLastConversionResults();                                      // conversion delay is included in this method
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_Differential(adsDiffMux_t regConfigDiffMUX) {
  // Start with default values
  uint16_t config = ADS1X15_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1X15_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;
  
  // Set Samples per Second
  config |= m_SPS;
                    
  // Set channels
  config |= regConfigDiffMUX;          // set P and N inputs for differential

  // Set 'start single-conversion' bit
  config |= ADS1X15_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1X15_REG_POINTER_CONFIG, config);
  
  // Wait for the conversion to complete
  waitForConversion();
  
  return getLastConversionResults();                                      // conversion delay is included in this method
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_Differential_0_1() {
  return readADC_Differential(DIFF_MUX_0_1);                               // AIN0 = P, AIN1 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN1) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_Differential_0_3() {
  return readADC_Differential(DIFF_MUX_0_3);                               // AIN0 = P, AIN3 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN1) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_Differential_1_3() {
  return readADC_Differential(DIFF_MUX_1_3);                               // AIN1 = P, AIN3 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::readADC_Differential_2_3() {
	return readADC_Differential(DIFF_MUX_2_3);                               // AIN2 = P, AIN3 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
			Applies the Volts per bit to return actual voltage
*/
/**************************************************************************/
float Adafruit_ADS1015::readADC_Differential_0_1_V() {
  return (float) readADC_Differential(DIFF_MUX_0_1) * voltsPerBit();                               // AIN0 = P, AIN1 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
			Applies the Volts per bit to return actual voltage
*/
/**************************************************************************/
float Adafruit_ADS1015::readADC_Differential_0_3_V() {
  return (float) readADC_Differential(DIFF_MUX_0_3) * voltsPerBit();                               // AIN0 = P, AIN1 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN1) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
			Applies the Volts per bit to return actual voltage
*/
/**************************************************************************/
float Adafruit_ADS1015::readADC_Differential_1_3_V() {
  return (float) readADC_Differential(DIFF_MUX_1_3) * voltsPerBit();                               // AIN0 = P, AIN1 = N
}

/**************************************************************************/
/*! 
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
			Applies the Volts per bit to return actual voltage
*/
/**************************************************************************/
float Adafruit_ADS1015::readADC_Differential_2_3_V() {
  return (float) readADC_Differential(DIFF_MUX_2_3) * voltsPerBit();                               // AIN0 = P, AIN1 = N
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.
*/
/**************************************************************************/
void Adafruit_ADS1015::startComparator_SingleEnded(uint8_t channel, int16_t threshold)
{
  // Start with default values
  uint16_t config = ADS1X15_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                    ADS1X15_REG_CONFIG_CLAT_LATCH   | // Latching mode
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1X15_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set PGA/voltage range
  config |= m_gain;
  
  // Set Samples per Second
  config |= m_SPS;
                    
  // Set single-ended input channel
  config |= getSingleEndedConfigBitsForMUX(channel);

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015
  writeRegister(m_i2cAddress, ADS1X15_REG_POINTER_HITHRESH, threshold << m_bitShift);
  
  // Set the high threshold register to the default
  writeRegister(m_i2cAddress, ADS1X15_REG_POINTER_LOWTHRESH, ADS1X15_LOW_THRESHOLD_DEFAULT);

  // Write config register to the ADC
  writeRegister(m_i2cAddress, ADS1X15_REG_POINTER_CONFIG, config);
  
}

/**************************************************************************/
/*!
    @brief  Poll the device each millisecond until the conversion is done.  
	        Using delay is important for an ESP8266 becasue it yeilds to the
			allow network operations to run.
*/
/**************************************************************************/
void Adafruit_ADS1015::waitForConversion()
{
  delay(0);                // delay(0) causes a yeild for ESP8266
  delayMicroseconds(10);   // Slight delay to ensure converstion started.  Probably not needed, but for safety
  do {
	  delay(0);            // delay(0) causes a yeild for ESP8266
	 } 
	 while (ADS1X15_REG_CONFIG_OS_BUSY == (readRegister(m_i2cAddress, ADS1X15_REG_POINTER_CONFIG) & ADS1X15_REG_CONFIG_OS_MASK));
            // Stop when the config register OS bit changes to 1
}

/**************************************************************************/
/*!
    @brief  This function reads the last conversion
            results without changing the config value.
			
			After the comparator triggers, in order to clear the comparator, 
			we need to read the conversion results.  
*/
/**************************************************************************/
int16_t Adafruit_ADS1015::getLastConversionResults()
{
  // Read the conversion results
  uint16_t res = readRegister(m_i2cAddress, ADS1X15_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == ADS1115_CONV_REG_BIT_SHIFT_0)            // for ADS1115
  {
    return (int16_t)res;
  }
  else
  {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF)
    {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*!
    @brief  Return the volts per bit for based on gain.  Multiply the adc
            reading by the value returned here to get actual volts. 
*/
/**************************************************************************/
float Adafruit_ADS1015::voltsPerBit()
{
	float v = 0;
	if (m_bitShift == ADS1015_CONV_REG_BIT_SHIFT_4) {            // for ADS1015
	  switch (m_gain)
	  {
		case (GAIN_TWOTHIRDS):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_TWOTHIRDS;
		  break;
		case (GAIN_ONE):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_ONE;
		  break;
		 case (GAIN_TWO):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_TWO;
		  break;
		case (GAIN_FOUR):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_FOUR;
		  break;
		case (GAIN_EIGHT):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_EIGHT;
		  break;
		case (GAIN_SIXTEEN):
		  v = ADS1015_VOLTS_PER_BIT_GAIN_SIXTEEN;
		  break;
	  }
	} else                  // for ADS1115
	{  
	  switch (m_gain)
	  {
		case (GAIN_TWOTHIRDS):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_TWOTHIRDS;
		  break;
		case (GAIN_ONE):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_ONE;
		  break;
		 case (GAIN_TWO):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_TWO;
		  break;
		case (GAIN_FOUR):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_FOUR;
		  break;
		case (GAIN_EIGHT):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_EIGHT;
		  break;
		case (GAIN_SIXTEEN):
		  v = ADS1115_VOLTS_PER_BIT_GAIN_SIXTEEN;
		  break;
	  }
	}
	return v;
}

