/**************************************************************************/
/*!
    @file     Adafruit_ADS1015.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    This is a library for the Adafruit ADS1015 breakout board
    ----> https://www.adafruit.com/products/???

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
    v1.1  - Added ADS1115 support - W. Earl
    v1.2  - Modified by Dennis Cabell.  See details in ccp file.
*/
/**************************************************************************/

#ifndef _ADAFRUIT_ADS1X15_H
#define _ADAFRUIT_ADS1X15_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define ADS1X15_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)
/*=========================================================================*/

/*=========================================================================
    Default thresholds for comparator
    -----------------------------------------------------------------------*/
    #define ADS1X15_LOW_THRESHOLD_DEFAULT   (0x8000)
    #define ADS1X15_HIGH_THRESHOLD_DEFAULT  (0x7FFF)
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1X15_REG_POINTER_MASK        (0x03)
    #define ADS1X15_REG_POINTER_CONVERT     (0x00)
    #define ADS1X15_REG_POINTER_CONFIG      (0x01)
    #define ADS1X15_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1X15_REG_POINTER_HITHRESH    (0x03)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1X15_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1X15_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1X15_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1X15_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1X15_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1X15_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1X15_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1X15_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1X15_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1X15_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1X15_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1X15_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1X15_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1X15_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1X15_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1X15_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1X15_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1X15_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1X15_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1X15_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1X15_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1X15_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1X15_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1015_REG_CONFIG_DR_MASK      (0x00E0)  
    #define ADS1015_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1015_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1015_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1015_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1015_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1015_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1015_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1115_REG_CONFIG_DR_8SPS      (0x0000)  // 8 samples per second
    #define ADS1115_REG_CONFIG_DR_16SPS     (0x0020)  // 16 samples per second
    #define ADS1115_REG_CONFIG_DR_32SPS     (0x0040)  // 32 samples per second
    #define ADS1115_REG_CONFIG_DR_64SPS     (0x0060)  // 64 samples per second
    #define ADS1115_REG_CONFIG_DR_128SPS    (0x0080)  // 128 samples per second (default)
    #define ADS1115_REG_CONFIG_DR_250SPS    (0x00A0)  // 250 samples per second
    #define ADS1115_REG_CONFIG_DR_475SPS    (0x00C0)  // 475 samples per second
    #define ADS1115_REG_CONFIG_DR_860SPS    (0x00E0)  // 860 samples per second

    #define ADS1X15_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1X15_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1X15_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1X15_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1X15_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1X15_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1X15_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1X15_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1X15_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1X15_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1X15_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1X15_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1X15_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1X15_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/

/*=========================================================================
    GAIN VOLTAGES
    -----------------------------------------------------------------------*/
    #define ADS1115_VOLTS_PER_BIT_GAIN_TWOTHIRDS   0.0001875F
    #define ADS1115_VOLTS_PER_BIT_GAIN_ONE         0.000125F  
    #define ADS1115_VOLTS_PER_BIT_GAIN_TWO         0.0000625F  
    #define ADS1115_VOLTS_PER_BIT_GAIN_FOUR        0.00003125F   
    #define ADS1115_VOLTS_PER_BIT_GAIN_EIGHT       0.000015625F  
    #define ADS1115_VOLTS_PER_BIT_GAIN_SIXTEEN     0.0000078125F 

    #define ADS1015_VOLTS_PER_BIT_GAIN_TWOTHIRDS   0.003F
    #define ADS1015_VOLTS_PER_BIT_GAIN_ONE         0.002F 
    #define ADS1015_VOLTS_PER_BIT_GAIN_TWO         0.001F  
    #define ADS1015_VOLTS_PER_BIT_GAIN_FOUR        0.0005F   
    #define ADS1015_VOLTS_PER_BIT_GAIN_EIGHT       0.00025F  
    #define ADS1015_VOLTS_PER_BIT_GAIN_SIXTEEN     0.000125F 
/*=========================================================================*/

/*=========================================================================
    CHIP BASED BIT SHIFT
    -----------------------------------------------------------------------*/
	#define ADS1015_CONV_REG_BIT_SHIFT_4           4
	#define ADS1115_CONV_REG_BIT_SHIFT_0           0

/*=========================================================================*/

typedef enum : uint16_t
{
  DIFF_MUX_0_1      = ADS1X15_REG_CONFIG_MUX_DIFF_0_1,
  DIFF_MUX_0_3      = ADS1X15_REG_CONFIG_MUX_DIFF_0_3,
  DIFF_MUX_1_3      = ADS1X15_REG_CONFIG_MUX_DIFF_1_3,
  DIFF_MUX_2_3      = ADS1X15_REG_CONFIG_MUX_DIFF_2_3
} adsDiffMux_t;

typedef enum : uint16_t
{
  GAIN_TWOTHIRDS    = ADS1X15_REG_CONFIG_PGA_6_144V,
  GAIN_ONE          = ADS1X15_REG_CONFIG_PGA_4_096V,
  GAIN_TWO          = ADS1X15_REG_CONFIG_PGA_2_048V,
  GAIN_FOUR         = ADS1X15_REG_CONFIG_PGA_1_024V,
  GAIN_EIGHT        = ADS1X15_REG_CONFIG_PGA_0_512V,
  GAIN_SIXTEEN      = ADS1X15_REG_CONFIG_PGA_0_256V,
  GAIN_DEFAULT      = ADS1X15_REG_CONFIG_PGA_6_144V
} adsGain_t;

typedef enum : uint16_t
{
    ADS1015_DR_128SPS          = ADS1015_REG_CONFIG_DR_128SPS,
    ADS1015_DR_250SPS          = ADS1015_REG_CONFIG_DR_250SPS,
    ADS1015_DR_490SPS          = ADS1015_REG_CONFIG_DR_490SPS,
    ADS1015_DR_920SPS          = ADS1015_REG_CONFIG_DR_920SPS,
    ADS1015_DR_1600SPS         = ADS1015_REG_CONFIG_DR_1600SPS,  // default for ADS1015
    ADS1015_DR_2400SPS         = ADS1015_REG_CONFIG_DR_2400SPS,
    ADS1015_DR_3300SPS         = ADS1015_REG_CONFIG_DR_3300SPS,

    ADS1115_DR_8SPS            = ADS1115_REG_CONFIG_DR_8SPS,
    ADS1115_DR_16SPS           = ADS1115_REG_CONFIG_DR_16SPS,
    ADS1115_DR_32SPS           = ADS1115_REG_CONFIG_DR_32SPS,
    ADS1115_DR_64SPS           = ADS1115_REG_CONFIG_DR_64SPS,
    ADS1115_DR_128SPS          = ADS1115_REG_CONFIG_DR_128SPS,   // default for ADS1115
    ADS1115_DR_250SPS          = ADS1115_REG_CONFIG_DR_250SPS,
    ADS1115_DR_475SPS          = ADS1115_REG_CONFIG_DR_475SPS,
    ADS1115_DR_860SPS          = ADS1115_REG_CONFIG_DR_860SPS,
	
	DR_DEFAULT_SPS             = (0x0080)     // 1600 for ADS1015, 128 for ADS1115
} adsSPS_t;

class Adafruit_ADS1015
{
protected:
   // Instance-specific properties
   uint8_t   m_i2cAddress;
   uint8_t   m_bitShift;
   adsGain_t m_gain                = GAIN_DEFAULT;  /* +/- 6.144V range (limited to VDD +0.3V max!) */
   adsSPS_t  m_SPS                 = DR_DEFAULT_SPS;

 public:
  Adafruit_ADS1015(uint8_t i2cAddress = ADS1X15_ADDRESS);
  void begin(void);
#if defined(ARDUINO_ARCH_ESP8266)
  void begin(uint8_t sda, uint8_t scl);
#endif  
  int16_t   readADC_SingleEnded(uint8_t channel);
  int16_t   readADC_Differential(adsDiffMux_t);
  int16_t   readADC_Differential_0_1(void);
  int16_t   readADC_Differential_0_3(void);
  int16_t   readADC_Differential_1_3(void);
  int16_t   readADC_Differential_2_3(void);
  void      startComparator_SingleEnded(uint8_t channel, int16_t highThreshold);
  void      startWindowComparator_SingleEnded(uint8_t channel, int16_t lowThreshold, int16_t highThreshold);
  void      startContinuous_SingleEnded(uint8_t channel);
  void      startContinuous_Differential(adsDiffMux_t);
  int16_t   getLastConversionResults(void);
  void      setGain(adsGain_t gain);
  adsGain_t getGain(void);
  void      setSPS(adsSPS_t gain);
  adsSPS_t  getSPS(void);
  float     voltsPerBit(void);
  float     readADC_SingleEnded_V(uint8_t);
  float     readADC_Differential_0_1_V(void);
  float     readADC_Differential_0_3_V(void);
  float     readADC_Differential_1_3_V(void);
  float     readADC_Differential_2_3_V(void);
  void      startContinuous_Differential_0_1(void);
  void      startContinuous_Differential_0_3(void);
  void      startContinuous_Differential_1_3(void);
  void      startContinuous_Differential_2_3(void);
  void      waitForConversion();

 private:

};

// Derive from ADS1105 & override construction to set properties
class Adafruit_ADS1115 : public Adafruit_ADS1015
{
 public:
  Adafruit_ADS1115(uint8_t i2cAddress = ADS1X15_ADDRESS);

 private:
};
#endif