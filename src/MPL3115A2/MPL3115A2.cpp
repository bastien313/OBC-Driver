#include "MPL3115A2.h"
 
/// @brief Instantiates a new MPL3115A2 class
MPL3115A2::MPL3115A2()
{
}
 
/// @brief Set I2C interface,
/// @param i2c I2C interface, must be previously intitialised.
void MPL3115A2::begin(TwoWire *i2c){
	m_i2c = i2c;
	init();
}

/// @brief Queries the value from the WHO_AM_I register (usually equal to 0xC4).
/// @return The fixed device ID from the sensor.
char MPL3115A2::whoAmI(void) 
{ 
	return i2cRead(WHO_AM_I); 
} 

/// @brief Initialise device with default value.
/// By default set the sensor to altimeter mode.
void MPL3115A2::init()
{
   
    i2cWrite(CTRL_REG1, 0x04);          // Reset all registers to POR values
    
    delay(1);
    
    i2cWrite(PT_DATA_CFG, 0x07);    // Enable data flags 
    i2cWrite(CTRL_REG3, 0x11);          // Push-pull, active low interrupt 
    i2cWrite(CTRL_REG4, 0x80);          // Enable DRDY interrupt 
    i2cWrite(CTRL_REG5, 0x00);          // DRDY interrupt routed to INT1 - PTA13 
    i2cWrite(CTRL_REG1, 0xB8);          // syandby, Altimeter mode, OSR = 128
}
 
/// @brief Wait while bit is set on status register of MPL3115A2
///		   This function return after 512 ms
/// @param mask mask of bit.
/// @return 1 if bit is set.
int MPL3115A2::dataReady(const char mask)
{
    int attempts = 0;
 
    while ((i2cRead(STATUS) & mask) == 0)
    {
        attempts++;
        
        if(attempts > MAX_DATA_READY_ATTEMPTS) 
            return 0; // Failed
            
        delay(1);
    }
    
    return 1; // Success
}

/// @brief Wait while pressure data ready bit is set
///			This function return after 512 ms
/// @return 1 if bit is set, 0 if timeout occur.
int MPL3115A2::pressureDataReady(void) 
{ 
	return dataReady(0x04); 
}

/// @brief Wait while temperature data ready bit is set
///			This function return after 512 ms
/// @return 1 if bit is set, 0 if timeout occur.
int MPL3115A2::temperatureDataReady(void) 
{ 
	return dataReady(0x02); 
}
 
/// @brief Read altitude from device and write data to Altitude pointer
///		   Before this method the device must be un barometer mode, call setModeAltimeter() if needed.
/// @param a Pointer of Altitude structure where we copy data
void MPL3115A2::readAltitude(Altitude* a)
{
	i2cRead(OUT_P_MSB, (*a), Altitude::size); // clear dataready bit
	toggleOneShot();
	delay(2);
	if (!pressureDataReady()){
		Serial.print("alt not ready");
	}
	i2cRead(OUT_P_MSB, (*a), Altitude::size);
	a->setAltitude();
}
 
/// @brief Read presure from device and write data to Presure pointer
///		   Before this method the device must be un barometer mode, call setModeBarometer() if needed.
/// @param a Pointer of Presure structure where we copy data
void MPL3115A2::readPressure(Pressure* p)
{	
	i2cRead(OUT_P_MSB, (*p), Pressure::size); // clear dataready bit
	toggleOneShot();
	delay(2);
	if (!pressureDataReady()){
		Serial.print("pres not ready");
	}
	i2cRead(OUT_P_MSB, (*p), Pressure::size);
	p->setPressure();
}
 
/// @brief Read temperature from device and write data to Temperature pointer
/// @param a Pointer of Temperature structure where we copy data
void MPL3115A2::readTemperature(Temperature* t)
{	
	i2cRead(OUT_T_MSB, (*t), (unsigned int)Temperature::size); // clear dataready bit
	toggleOneShot();
	delay(2);
	if (!temperatureDataReady()){
	}
	i2cRead(OUT_T_MSB, (*t), (unsigned int)Temperature::size);
	t->setTemperature();
}
 
/// @brief Put device in altimeter mode.
/// This method slow down the device, use it only if needed.
void MPL3115A2::setModeAltimeter()
{
	setRegisterBit(CTRL_REG1, 0x80);// Set ALT bit  
}
 
/// @brief Put device in barometer mode.
/// This method slow down the device, use it only if needed.
void MPL3115A2::setModeBarometer()
{
	clearRegisterBit(CTRL_REG1, 0x80);// Clear ALT bit
    //i2cWrite(CTRL_REG1, 0x39);  
}
 
/// @brief Put device in standby mode.
void MPL3115A2::setModeStandby()
{
	clearRegisterBit(CTRL_REG1, 0x01);// Clear SBYB bit for Standby mode
    //i2cWrite(CTRL_REG1, 0x01);  
}

/// @brief Put device in active mode.
void MPL3115A2::setModeActive()
{
	setRegisterBit(CTRL_REG1, 0x80);// Set SBYB bit for Active mode
    //i2cWrite(CTRL_REG1, 0x01);    
}

/// @brief Sets the temperature offset stored in the sensor. The allowed offset range is from -128 to 127 where each LSB represents 0.0625ยบC.
/// @param offset temperature offset -128 to 127
void MPL3115A2::setOffsetTemperature(const char offset) 
{ 
	i2cWrite(OFF_T, offset); 
} 

/// @brief Sets the pressure offset stored in the sensor. The allowed offset range is from -128 to 127 where each LSB represents 4 Pa.
/// @param offset presure offset -128 to 127
void MPL3115A2::setOffsetPressure(const char offset) 
{ 
	i2cWrite(OFF_P, offset); 
}

/// @briefSets the altitude offset stored in the sensor. The allowed offset range is from -128 to 127 meters.
/// @param offset altitude offset -128 to 127
void MPL3115A2::setOffsetAltitude(const char offset)
{ 
	i2cWrite(OFF_H, offset); 
}

/// @brief Returns the temperature offset stored in the sensor.
/// @return Returns the temperature offset stored in the sensor.
char MPL3115A2::offsetTemperature(void) 
{ 
	return i2cRead(OFF_T); 
}

/// @brief Returns the pressure offset stored in the sensor.
/// @return Returns the pressure offset stored in the sensor.
char MPL3115A2::offsetPressure(void) 
{ 
	return i2cRead(OFF_P); 
}

/// @brief Returns the altitude offset stored in the sensor.
/// @return Returns the altitude offset stored in the sensor.
char MPL3115A2::offsetAltitude(void) 
{ 
	return i2cRead(OFF_H); 
}

/// @brief  Sets the number of samples from 1 to 128, be sure to put the sensor in standby mode first.
/// @param sampleRate Sample rate 0-128
void MPL3115A2::setOversampleRate(char sampleRate)
{
    if(sampleRate > 7) 
        sampleRate = 7;                 // OS cannot be larger than 0b.0111
    
    sampleRate <<= 3;                   // Align it for the CTRL_REG1 register
  
    char temp = i2cRead(CTRL_REG1);     // Read current settings
    temp &= 0xC7;                       // Clear out old OS bits
    temp |= sampleRate;                 // Mask in new OS bits
    i2cWrite(CTRL_REG1, temp);
}

/// @brief Sets all the event flags, be sure to put the sensor in standby mode first.
void MPL3115A2::enableEventFlags()
{
    i2cWrite(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}
 
/// @brief Force the sensor to take another sample
void MPL3115A2::toggleOneShot(void)
{
    clearRegisterBit(CTRL_REG1, 0x02);  // Clear OST bit
    setRegisterBit(CTRL_REG1, 0x02);    // Set the OST bit.
}

/// @brief This helper function is used to CLEAR bits. The mask should contain 1s in the position
///        where the bits need to be cleared. One or more bits can be cleared this way.
/// @param regAddr Address of register
/// @param bitMask Bit mask to clear
void MPL3115A2::clearRegisterBit(const char regAddr, const char bitMask)
{
    char temp = i2cRead(regAddr);   // Read the current register value
    temp &= ~bitMask;               // Clear the bit from the value
    i2cWrite(regAddr, temp);        // Write register value back
}

/// @brief This helper function is used to SET bits. The mask should contain 1s in the position
///        where the bits need to be set. One or more bits can be set this way.
/// @param regAddr Address of register
/// @param bitMask Bit mask to clear
void MPL3115A2::setRegisterBit(const char regAddr, const char bitMask)
{
    char temp = i2cRead(regAddr);   // Read the current register value
    temp |= bitMask;                // Set the bit in the value
    i2cWrite(regAddr, temp);        // Write register value back
}

/// @brief    Helper functions to read one bytes from the I2C bus using the sensor's address.
/// @param regAddr Address of register
/// @return The value of readed register.
char MPL3115A2::i2cRead(char regAddr)
{
	m_i2c->requestFrom(MPL3115A2_ADDRESS,1, regAddr,1,true);
	return m_i2c->read();
}

/// @brief    Helper functions to read multiple byte from the I2C bus using the sensor's address.
/// @param regAddr Address of register
/// @param buffer Pointer where data will be send
/// @param size Size to read
void MPL3115A2::i2cRead(char regAddr, char *buffer, unsigned int size)
{
	m_i2c->requestFrom(MPL3115A2_ADDRESS,size, regAddr, 1,true);
	for(unsigned int i = 0; i<size; i++){
		buffer[i] = m_i2c->read();
	}
}
 
/// @brief  Helper functions to write one value from the I2C bus using the sensor's address.
/// @param regAddr Address of register
/// @param value Value to write on register
void MPL3115A2::i2cWrite(char regAddr, char value)
{	
	m_i2c->beginTransmission(MPL3115A2_ADDRESS);
    m_i2c->write(&regAddr, 1);
	m_i2c->write(&value,1);
	m_i2c->endTransmission();

}
 
