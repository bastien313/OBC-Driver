/**************************************************************************/
/*!
    @file     ADS1015.cpp
    @author   K.Townsend (Adafruit Industries)
    @license  BSD (see LICENSE.txt)

    Ported to mbed by Arve Seljebu - arve0.github.io

    Driver for the ADS1015/ADS1115 ADC

    This is a library for the Adafruit MPL115A2 breakout
    ----> https://www.adafruit.com/products/1083

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
    v1.1.1 - Ported to mbed
	v1.2 - Ported to arduino -  Sclafer Bastien.
*/
/**************************************************************************/

#include <cmath>
#include "ADS1015.h"
#include <Wire.h>

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
void ADS1015::writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value)
{
	m_i2c->beginTransmission(i2cAddress);
    char cmd[3];
    cmd[0] = (char)reg;
    cmd[1] = (char)(value>>8);
    cmd[2] = (char)(value & 0xFF);
    m_i2c->write(cmd, 3);
	m_i2c->endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified register
*/
/**************************************************************************/
uint16_t ADS1015::readRegister(uint8_t i2cAddress, uint8_t reg)
{
	
    char data[2];
    data[0] = reg; // temporary use this to send address to conversion register
	m_i2c->beginTransmission(i2cAddress);
    m_i2c->write(data, 1);
	m_i2c->endTransmission();
	
	m_i2c->requestFrom(i2cAddress,2);
	if (m_i2c->available()) {
		uint8_t firstByte = m_i2c->read();
		uint8_t secondByte = m_i2c->read();

		return (firstByte << 8) + secondByte;
	}
    return 0;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1015 class w/appropriate properties
*/
/**************************************************************************/
ADS1015::ADS1015()
{
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1115 class w/appropriate properties
*/
/**************************************************************************/
ADS1115::ADS1115()
{
}

void ADS1015::begin(TwoWire* i2c, uint8_t i2cAddress){
	m_i2cAddress = i2cAddress ;//<< 1;
    m_conversionDelay = ADS1015_CONVERSIONDELAY;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
    m_i2c = i2c;
}

/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range
*/
/**************************************************************************/
void ADS1015::setGain(adsGain_t gain)
{
    m_gain = gain;
}

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range
*/
/**************************************************************************/
adsGain_t ADS1015::getGain()
{
    return m_gain;
}

/**************************************************************************/
/*!
    @brief  Retur true if conversion is on going.
*/
/**************************************************************************/
bool ADS1015::conversionOnGoing(void){
	if( readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG) & ADS1015_REG_CONFIG_OS_NOTBUSY){
		return false;
	}else{
		return true;
	}
}

/**************************************************************************/
/*!
    @brief  Wait end of conversion.
	return false if timeout occur.
*/
/**************************************************************************/
bool ADS1015::waitEndOfConversion(uint32_t timeoutMs){
	unsigned long inTime = millis();
	while(conversionOnGoing()){
		if((millis() - inTime) > timeoutMs){
			return false;
		}
	}
	return true;
}

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel
*/
/**************************************************************************/
int16_t ADS1015::readADC_SingleEnded(uint8_t channel)
{
    if (channel > 3) {
        return 0;
    }

    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_3300SPS   | // 860(ADS1115) samples per second
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set single-ended input channel
    switch (channel) {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
	waitEndOfConversion();

    // Read the conversion results
	uint16_t rawResult = readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT);
    return *((int16_t*)(&rawResult));
//    return 1;
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1015::readADC_Differential_0_3()
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_3300SPS   | // 1600(ADS1015) or 250(ADS1115) samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;          // AIN0 = P, AIN1 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    waitEndOfConversion();

    // Read the conversion results
	uint16_t rawResult = readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT);
    return *((int16_t*)(&rawResult));

}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1015::readADC_Differential_1_3()
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_3300SPS   | // 1600(ADS1015) or 250(ADS1115) samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_1_3;          // AIN0 = P, AIN1 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    waitEndOfConversion();

    // Read the conversion results
	uint16_t rawResult = readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT);
    return *((int16_t*)(&rawResult));

}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.
*/
/**************************************************************************/
void ADS1015::startComparator_SingleEnded(uint8_t channel, int16_t threshold)
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                      ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching mode
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600(ADS1015) or 250(ADS1115) samples per second (default)
                      ADS1015_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
                      ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

    // Set PGA/voltage range
    config |= m_gain;

    // Set single-ended input channel
    switch (channel) {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set the high threshold register
    // Shift 12-bit results left 4 bits for the ADS1015
    writeRegister(m_i2cAddress, ADS1015_REG_POINTER_HITHRESH, threshold);

    // Write config register to the ADC
    writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.
*/
/**************************************************************************/
int16_t ADS1015::getLastConversionResults()
{
    // Wait for the conversion to complete
    waitEndOfConversion();

    // Read the conversion results
	uint16_t rawResult = readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT);
    return *((int16_t*)(&rawResult));

}





float ADS1015::getVref(void){
	if(m_gain == GAIN_TWOTHIRDS){
		return 6.144f;
	}else if (m_gain == GAIN_ONE){
		return 4.096f;
	}else if (m_gain == GAIN_TWO){
		return 2.048f;
	}else if (m_gain == GAIN_FOUR){
		return 1.024f;
	}else if (m_gain == GAIN_EIGHT){
		return 0.512f;
	}else{
		return 0.256f;
	}
}

/*********************************************************************************/
/*!
    @brief  Conversion function for the ADC.
*/
/*********************************************************************************/
float_t ADS1015::ADC_Conversion(int16_t raw_Vtemp)
{
    return (float)raw_Vtemp * (getVref() / 32767.5f);
}
