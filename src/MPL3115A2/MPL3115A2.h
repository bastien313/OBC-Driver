/*
    MPL3115A2 Barometric Pressure and Tempurature Sensor Library
    By: Michael Lange
    Date: March 31, 2014
    License: This code is public domain.
 
    This class wraps the functions of the MPL3115A2 sensor into 
    a usable class that exposes most functions to your project.
    Functions that are not exposed can easily be added using the
    existing functions in this library. Specifically, I did not
    add any funtions to handle FIFO logging or the use of either
    of the pin interrupts. This should not be too difficult to
    add if you need those features.
    
    The motivation here was to get a set of support classes 
    together that supported the chip and could be expanded on.
    With this library you can extract all relevant data from the
    sensor.
    
    Be sure to download the DATASHEET and the App Note AN4519.
    
    This library was created using the mbed NXP LPC11U24. Pins
    p27 and p28 were used for the I2C functions. Be sure to install
    1K pull-up resistors on both lines. Also, if you're not using
    the SparkFun breakout board, be sure to use the right caps on 
    the power pin. If you don't, the jitter can cause problems.
    
    This library was inspired by the similar library available for
    the Arduino written by Nathan Seidle at SparkFun. I copied
    some of the number crunching routines and tried to follow his
    style of coding for the library. That way users of Arduinos
    could step into this library a little easier.
 
 */
 
 
#ifndef MPL3115A2_H
#define MPL3115A2_H
 
#include <Wire.h>
 
#include "Altitude.h"               // Our classes to handle compressed data from the sensor.
#include "Temperature.h"
#include "Pressure.h"
 
#define MPL3115A2_ADDRESS   0x60    // 7-bit I2C address for sensor
 
#define READ_ACK 1                  // For mbed I2C Read method.
#define READ_NAK 0 
 
#define MAX_DATA_READY_ATTEMPTS 512 // How many times we loop waiting for data before giving up.
 
//      DEFINE             |REGISTER |RESET |RESET    |TYPE |AUTO-INC      |NAME/COMMENT
//                         |         |      |STBY2ACT |     |ADDRESS       |
#define STATUS              0x00 //  | 0x00 | Yes     | R   | 0x01         | Sensor Status Register (Alias for DR_STATUS or F_STATUS)
#define OUT_P_MSB           0x01 //  | 0x00 | Yes     | R   | 0x02 | 0x01  | Pressure Data Out MSB (Bits 12-19 of 20-bit real-time Pressure sample | Root pointer t oPressure and Tempurature FIFO data)
#define OUT_P_CSB           0x02 //  | 0x00 | Yes     | R   | 0x03         | Pressure Data out CSB (Bits 0-3 of 20-bit real-time Pressure sample)
#define OUT_P_LSB           0x03 //  | 0x00 |         |     | 0x           |
#define OUT_T_MSB           0x04 //  | 0x00 |         |     | 0x           |
#define OUT_T_LSB           0x05 //  | 0x00 |         |     | 0x           |
#define DR_STATUS           0x06 //  | 0x00 |         |     | 0x           |
#define OUT_P_DELTA_MSB     0x07 //  | 0x00 |         |     | 0x           |
#define OUT_P_DELTA_CSB     0x08 //  | 0x00 |         |     | 0x           |
#define OUT_P_DELTA_LSB     0x09 //  | 0x00 |         |     | 0x           |
#define OUT_T_DELTA_MSB     0x0A //  | 0x00 |         |     | 0x           |
#define OUT_T_DELTA_LSB     0x0B //  | 0x00 |         |     | 0x           |
#define WHO_AM_I            0x0C //  | 0xC4 |         |     | 0x           |
#define F_STATUS            0x0D //  | 0x00 |         |     | 0x           |
#define F_DATA              0x0E //  | 0x00 |         |     | 0x           |
#define F_SETUP             0x0F //  | 0x00 |         |     | 0x           |
#define TIME_DLY            0x10 //  | 0x00 |         |     | 0x           |
#define SYSMOD              0x11 //  | 0x00 |         |     | 0x           |
#define INT_SOURCE          0x12 //  | 0x00 |         |     | 0x           |
#define PT_DATA_CFG         0x13 //  | 0x00 |         |     | 0x           |
#define BAR_IN_MSB          0x14 //  | 0xC5 |         |     | 0x           |
#define BAR_IN_LSB          0x15 //  | 0xE7 |         |     | 0x           |
#define P_TGT_MSB           0x16 //  | 0x00 |         |     | 0x           |
#define P_TGT_LSB           0x17 //  | 0x00 |         |     | 0x           |
#define T_TGT               0x18 //  | 0x00 |         |     | 0x           |
#define P_WND_MSB           0x19 //  | 0x00 |         |     | 0x           |
#define P_WND_LSB           0x1A //  | 0x00 |         |     | 0x           |
#define T_WND               0x1B //  | 0x00 |         |     | 0x           |
#define P_MIN_MSB           0x1C //  | 0x00 |         |     | 0x           |
#define P_MIN_CSB           0x1D //  | 0x00 |         |     | 0x           |
#define P_MIN_LSB           0x1E //  | 0x00 |         |     | 0x           |
#define T_MIN_MSB           0x1F //  | 0x00 |         |     | 0x           |
#define T_MIN_LSB           0x20 //  | 0x00 |         |     | 0x           |
#define P_MAX_MSB           0x21 //  | 0x00 |         |     | 0x           |
#define P_MAX_CSB           0x22 //  | 0x00 |         |     | 0x           |
#define P_MAX_LSB           0x23 //  | 0x00 |         |     | 0x           |
#define T_MAX_MSB           0x24 //  | 0x00 |         |     | 0x           |
#define T_MAX_LSB           0x25 //  | 0x00 |         |     | 0x           |
#define CTRL_REG1           0x26 //  | 0x00 |         |     | 0x           |
#define CTRL_REG2           0x27 //  | 0x00 |         |     | 0x           |
#define CTRL_REG3           0x28 //  | 0x00 |         |     | 0x           |
#define CTRL_REG4           0x29 //  | 0x00 |         |     | 0x           |
#define CTRL_REG5           0x2A //  | 0x00 |         |     | 0x           |
#define OFF_P               0x2B //  | 0x00 |         |     | 0x           |
#define OFF_T               0x2C //  | 0x00 |         |     | 0x           |
#define OFF_H               0x2D //  | 0x00 |         |     | 0x           |
 
//! MPL3115A2 I2C Barometric Pressure and Tempurature Sensor Library
//! This class wraps most of the function in the MPL3115A2 sensor leaving out the FIFO and interrupt system.
class MPL3115A2
{
public:
    MPL3115A2();
	void begin(TwoWire *i2c);

    void init(void);
 
    char  whoAmI(void);
    
    void readAltitude(Altitude* a);            
    void readPressure(Pressure* p);            
    void readTemperature(Temperature* t);
    
    /// Use these methods to set the sensor's offsets to increase its accuracy. You can generally
    /// find your current altitude with a smart phone or GPS. Same goes for the temperature. For
    /// the current pressure where you are, you may need an accurate weather station. Getting the
    /// pressure from the web for your area is generally not close enough to help with calibration.
    /// You may need to play with the setting to achieve good accuracy. I found the offset steps
    /// were not 100% accurate to the datasheet and had to adjust accordingly. 
    void setOffsetTemperature(const char offset);
    void setOffsetPressure(const char offset);
    void setOffsetAltitude(const char offset);
    char offsetTemperature(void);
    char offsetPressure(void);
    char offsetAltitude(void);

    void  setModeStandby(void); 
    void  setModeActive(void);  
    void  setModeBarometer(void);           
    void  setModeAltimeter(void);           
    void  setOversampleRate(char rate); 
    void  enableEventFlags(void);           
 
private:
    TwoWire *m_i2c;          
    void clearRegisterBit(const char regAddr, const char bitMask);

    void setRegisterBit(const char regAddr, const char bitMask);
    int dataReady(const char mask);
    int pressureDataReady(void);
    int temperatureDataReady(void);
    void toggleOneShot(void);                       
    
    char i2cRead(char regAddr);
	void i2cRead(char regAddr, char *buffer, unsigned int size);
    void i2cWrite(char regAddr, char value);
};
 
#endif // MPL3115A2_H