/// @example EPS.ino
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
  // The standard usage is to call the get() function and read all the received data using the member 
  // vbat, vin, vout, ibat, iin, die_temp, temperature;
  OBC.EPS.get();

  Serial.print("Vbat = ");
  Serial.println(OBC.EPS.vbat);

  Serial.print("vin = ");
  Serial.println(OBC.EPS.vin);

  Serial.print("vout = ");
  Serial.println(OBC.EPS.vout);

  Serial.print("ibat = ");
  Serial.println(OBC.EPS.ibat);

  Serial.print("iin = ");
  Serial.println(OBC.EPS.iin);

  Serial.print("die_temp = ");
  Serial.println(OBC.EPS.die_temp);

  Serial.print("temperature = ");
  Serial.println(OBC.EPS.temperature);

  Serial.println("-------");

  delay(1000);
}