#include "MPL3115A2.h"
 
 
MPL3115A2::MPL3115A2()
{
}
 
void MPL3115A2::begin(TwoWire *i2c){
	m_i2c = i2c;
	init();
}
 
// By default I set the sensor to altimeter mode. I inserted a 1ms pause 
// between each call to allow logic capture if needed. This give a small
// gap between captures on the bus to make working with the data easier.
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
 
// This method wait for a specified amount of time for one of the data
// ready flags to be set. You need to pass in the correct data ready
// mask for this to work. See page 22 of the datasheet.
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
 
Altitude* MPL3115A2::readAltitude(Altitude* a)
{
	toggleOneShot();
	delay(2);
	if (!pressureDataReady()){
		Serial.print("alt not ready");
		return a;
	}
	i2cRead(OUT_P_MSB, (*a), Altitude::size);
	a->setAltitude();
	return a;
}
 
// See readAltitude for comments about this function.
Pressure* MPL3115A2::readPressure(Pressure* p)
{	
	toggleOneShot();
	if (!pressureDataReady()){
		Serial.print("pres not ready");
		return p;
	}
	i2cRead(OUT_P_MSB, (*p), Pressure::size);
	p->setPressure();
	return p;
}
 
// See readAltitude for comments about this function.
Temperature* MPL3115A2::readTemperature(Temperature* t)
{	
	toggleOneShot();
	if (!temperatureDataReady()){
		return t;
	}
	i2cRead(OUT_T_MSB, (*t), (unsigned int)Temperature::size);
	t->setTemperature();
	return t;
}
 
void MPL3115A2::setModeAltimeter()
{
	setRegisterBit(CTRL_REG1, 0x80);// Set ALT bit
    //i2cWrite(CTRL_REG1, 0xB9);    
}
 
void MPL3115A2::setModeBarometer()
{
	clearRegisterBit(CTRL_REG1, 0x80);// Clear ALT bit
    //i2cWrite(CTRL_REG1, 0x39);  
}
 
void MPL3115A2::setModeStandby()
{
	clearRegisterBit(CTRL_REG1, 0x01);// Clear SBYB bit for Standby mode
    //i2cWrite(CTRL_REG1, 0x01);  
}
void MPL3115A2::setModeActive()
{
	setRegisterBit(CTRL_REG1, 0x80);// Set SBYB bit for Active mode
    //i2cWrite(CTRL_REG1, 0x01);    
}
 
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
 
void MPL3115A2::enableEventFlags()
{
    i2cWrite(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}
 
void MPL3115A2::toggleOneShot(void)
{
    clearRegisterBit(CTRL_REG1, 0x02);  // Clear OST bit
    setRegisterBit(CTRL_REG1, 0x02);    // Set the OST bit.
}
 
void MPL3115A2::clearRegisterBit(const char regAddr, const char bitMask)
{
    char temp = i2cRead(regAddr);   // Read the current register value
    temp &= ~bitMask;               // Clear the bit from the value
    i2cWrite(regAddr, temp);        // Write register value back
}
 
void MPL3115A2::setRegisterBit(const char regAddr, const char bitMask)
{
    char temp = i2cRead(regAddr);   // Read the current register value
    temp |= bitMask;                // Set the bit in the value
    i2cWrite(regAddr, temp);        // Write register value back
}
 
char MPL3115A2::i2cRead(char regAddr)
{
	m_i2c->requestFrom(MPL3115A2_ADDRESS,1, regAddr,1,true);
	return m_i2c->read();
}

void MPL3115A2::i2cRead(char regAddr, char *buffer, unsigned int size)
{
	m_i2c->requestFrom(MPL3115A2_ADDRESS,size, regAddr, 1,true);
	for(unsigned int i = 0; i<size; i++){
		buffer[i] = m_i2c->read();
	}
}
 
void MPL3115A2::i2cWrite(char regAddr, char value)
{	
	m_i2c->beginTransmission(MPL3115A2_ADDRESS);
    m_i2c->write(&regAddr, 1);
	m_i2c->write(&value,1);
	m_i2c->endTransmission();

}
 
