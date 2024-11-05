/// @example LTC4162.ino
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
  // Read temperature Reads all relevant data from the device result can be read from:
  //- OBC.EPS.vbat
  //- OBC.EPS.vin
  //- OBC.EPS.vout
  //- OBC.EPS.iin
  //- OBC.EPS.temperature
  //- OBC.EPS.die_temp
  //- OBC.EPS.ibat
  OBC.EPS.get();

  //Print Device data data
  Serial.print("EPS die Temperature = ");
  Serial.print(OBC.EPS.die_temp);
  Serial.println("°C");

  Serial.print("Battery Temperature = ");
  Serial.print(OBC.EPS.temperature);
  Serial.println("°C");

  Serial.print("Battery volatge = ");
  Serial.print(OBC.EPS.vbat);
  Serial.println("V");

  Serial.print("Input volatge = ");
  Serial.print(OBC.EPS.vin);
  Serial.println("V");

  Serial.print("Output volatge = ");
  Serial.print(OBC.EPS.vout);
  Serial.println("V");

  Serial.print("Input Current = ");
  Serial.print(OBC.EPS.iin);
  Serial.println("A");

  Serial.print("Battery Current = ");
  Serial.print(OBC.EPS.ibat);
  Serial.println("A");
  Serial.println(" ");


  delay(1000);
}