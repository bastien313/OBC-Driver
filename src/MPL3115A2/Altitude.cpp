#include "Altitude.h"
 #include <Arduino.h>
Altitude::Altitude()
{
    _altitude = 0.0;
 
    _compressed[0] = 0;
    _compressed[1] = 0;
    _compressed[2] = 0;
}
 
Altitude::Altitude(float a, unitsType units)
{
    setAltitude(a, units);
}
    
Altitude::Altitude(const char* compressed)
{
    setAltitude(compressed);
}
 
Altitude::Altitude(const char msb, const char csb, const char lsb)
{
    setAltitude(msb, csb, lsb);
}
 
void Altitude::setAltitude()
{
    setAltitude(_compressed[0], _compressed[1], _compressed[2]);
}
 
void Altitude::setAltitude(float a, unitsType units)
{
    // TODO:
}
    
void Altitude::setAltitude(const char* compressed)
{
    setAltitude(compressed[0], compressed[1], compressed[2]);
}
 
void Altitude::setAltitude(const char msb, const char csb, const char lsb)
{	
	int16_t alt_whole = (*((uint8_t*)&msb))<<8 | (*((uint8_t*)&csb));
	float alt_decimal = (float)((*((uint8_t*)&lsb)) >> 4);
	alt_decimal /= 1000.0f;
    _altitude = ((float)alt_whole)+alt_decimal;
}
 
float Altitude::altitude(unitsType units)
{
    switch (units)
    {
        case FEET:
            return MetersToFeet(_altitude);
        case METERS:
            return (_altitude);
    }
    
    return _altitude;
}
 