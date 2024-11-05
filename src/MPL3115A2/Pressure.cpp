#include "Pressure.h"
#include <Arduino.h>


/// @brief Instantiates a new Pressure class with default value (0)
Pressure::Pressure()
{
    _pressure = 0.0;
 
    _compressed[0] = 0;
    _compressed[1] = 0;
    _compressed[2] = 0;
}

/*
Pressure::Pressure(float a, unitsType units)
{
    setPressure(a, units);
}*/
    
/// @brief Instantiates a new Pressure class and parse value 
/// @param compressed Pointer where bytes data is located.
Pressure::Pressure(const char* compressed)
{
    setPressure(compressed);
}
 
/// @brief Instantiates a new Pressure class and parse value 
/// @param msb msb
/// @param csb csb
/// @param lsb lsb
Pressure::Pressure(const char msb, const char csb, const char lsb)
{
    setPressure(msb, csb, lsb);
}
 

/// @brief Parse intern buffer for extract presure.
void Pressure::setPressure()
{
    setPressure(_compressed[0], _compressed[1], _compressed[2]);
}
 
/*
void Pressure::setPressure(float a, unitsType units)
{
    // TODO:
}*/

/// @brief Parse extern buffer for extract altitude.
/// @param compressed extern buffer 
void Pressure::setPressure(const char* compressed)
{
    setPressure(compressed[0], compressed[1], compressed[2]);
}
 
/// @brief Parse msb, csb, lsb for extract altitude.
/// @param compressed extern buffer
void Pressure::setPressure(const char msb, const char csb, const char lsb)
{

    /*long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
    pressure_whole >>= 6; // Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
 
    float pressure_decimal = (float)((lsb&0x30)>>4)/4.0; // Turn it into fraction*/
	uint32_t pressure_whole = (*((uint8_t*)&msb))<<16 | (*((uint8_t*)&csb))<<8 | (*((uint8_t*)&lsb));
	pressure_whole >>= 6; // Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
	float pressure_decimal = (float)(((*((uint8_t*)&lsb))&0x30)>>4)/4.0; // Turn it into fraction*/
    _pressure = (float)pressure_whole + pressure_decimal;
}
 
/// @brief Get Presure value
/// @param units units
/// @return Altitude value
float Pressure::pressure(unitsType units)
{
    // http://www.asknumbers.com/
    switch (units)
    {
        case PSI:
            return _pressure * 0.000145037738;    
        case INHG:
            return _pressure * 0.00029529983071;
        case MMHG:
            return _pressure * 0.007500615613;
    }
    
    return _pressure;
}
 