#include "Temperature.h"
#include "Arduino.h"

/// @brief Instantiates a new Temperature class with default value (0)
Temperature::Temperature()
{
    _temperature = 0.0;
 
    _compressed[0] = 0;
    _compressed[1] = 0;
}

/*
Temperature::Temperature(float t, unitsType units)
{
    setTemperature(t, units);
}*/

/// @brief Instantiates a new Temperature class and parse value 
/// @param compressed Pointer where bytes data is located.   
Temperature::Temperature(const char* compressed)
{
    setTemperature(compressed);
}

/// @brief Instantiates a new Temperature class and parse value 
/// @param msb msb
/// @param lsb lsb
Temperature::Temperature(const char msb, const char lsb)
{
    setTemperature(msb, lsb);
}
 
/// @brief Parse intern buffer for extract altitude.
void Temperature::setTemperature()
{
    setTemperature(_compressed[0], _compressed[1]);
}

/*
void Temperature::setTemperature(float t, unitsType units)
{
    // TODO:
}*/

/// @brief Parse extern buffer for extract temperature.
/// @param compressed extern buffer  
void Temperature::setTemperature(const char* compressed)
{
    setTemperature(compressed[0], compressed[1]);
}

/// @brief Parse msb, csb, lsb for extract temperature.
/// @param compressed extern buffer
void Temperature::setTemperature(const char msb, const char lsb)
{
    float templsb = (lsb>>4) / 16.0; //temp, fraction of a degree
    _temperature = (float)(msb + templsb);
}

/// @brief Get temperature value
/// @param units units
/// @return Altitude value
float Temperature::temperature(unitsType units)
{
    switch (units)
    {
        case FAHRENHEIT:
            return (_temperature * 9.0)/ 5.0 + 32.0;
        
        case KELVIN:
            return _temperature + 273.15;
        case CELSIUS:
             return _temperature;
    }
    
    return _temperature;
}
 
