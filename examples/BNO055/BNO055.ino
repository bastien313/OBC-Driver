/// @example BNO055.ino
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
  char buffOut[256]; // Buffer for serial output
  // Read temperature from BNO055, result can be read from OBC.BNO.temperature
  OBC.BNO.get_temp();

  //Print temperature data
  sprintf(buffOut, "BNO Temperature = %d°C", OBC.BNO.temperature);
  Serial.println(buffOut);

  //Set acceleration units in mG.
  OBC.BNO.set_accel_units(MILLIG);
  // Read current values from the accelerometer from BNO055, result can be read from OBC.BNO.accel
  OBC.BNO.get_accel();

  //Print acceleration data
  Serial.print("BNO Accel:  X=");
  Serial.print(OBC.BNO.accel.x);
  Serial.print("mG Y=");
  Serial.print(OBC.BNO.accel.y);
  Serial.print("mG Z=");
  Serial.print(OBC.BNO.accel.z);
  Serial.println("mG");

  delay(1000);
}