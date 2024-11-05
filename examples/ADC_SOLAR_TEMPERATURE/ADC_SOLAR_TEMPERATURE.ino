/// @example ADC_SOLAR_TEMPERATURE.ino
#include <OBC_Arduino.h>
#include <string.h>

OBC_Arduino OBC;

void setup() {
  //Debuging use.
  Serial.begin(); 

  //Initialise OBC library
  OBC.begin(); 
}

void loop() {

  // Read solar voltage
  float xp = OBC.XP_readVolatge();
  float xn = OBC.XN_readVolatge();
  float yp = OBC.YP_readVolatge();
  float yn = OBC.YN_readVolatge();
  float z = OBC.Z_readVolatge();

  Serial.print("X+ = ");
  Serial.print(xp);
  Serial.println("V");

  Serial.print("X- = ");
  Serial.print(xn);
  Serial.println("V");

  Serial.print("Y+ = ");
  Serial.print(yp);
  Serial.println("V");

  Serial.print("Y- = ");
  Serial.print(yn);
  Serial.println("V");

  Serial.print("Z = ");
  Serial.print(z);
  Serial.println("V");

  // Read thermisance temperature
  float voltage = OBC.thermistor_readVolatge(0);
  // Temperature conversion
  float temperature = OBC_Arduino::thermistor_voltageConversion(voltage);
  Serial.print("Temperature 1 = ");
  Serial.print(temperature);
  Serial.println("°C");

  // Read thermisance temperature
  voltage = OBC.thermistor_readVolatge(1);
  // Temperature conversion
  temperature = OBC_Arduino::thermistor_voltageConversion(voltage);
  Serial.print("Temperature 2 = ");
  Serial.print(temperature);
  Serial.println("°C");

  // Read thermisance temperature
  voltage = OBC.thermistor_readVolatge(2);
  // Temperature conversion
  temperature = OBC_Arduino::thermistor_voltageConversion(voltage);
  Serial.print("Temperature 3 = ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.println("----------");

  delay(1000);
}