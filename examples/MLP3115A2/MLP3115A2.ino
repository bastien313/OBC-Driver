/// @example MLP3115A2.ino
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

  /// Altitude reading
  // Instantiate Altitude parser
  Altitude A;
  // Set altimeter mode
  OBC.MPL.setModeAltimeter();
  // Read altitude
  OBC.MPL.readAltitude(&A);
  // Parse altitude with selected unit
  float altitude = A.altitude(Altitude::METERS);
  Serial.print("Altitude = ");
  Serial.print(altitude);
  Serial.println("M");

  /// Pressure reading
  // Instantiate pressure parser
  Pressure P;
  // Set barometer mode
  OBC.MPL.setModeBarometer();
  // Read pressure
  OBC.MPL.readPressure(&P);
  // Parse altitude with selected unit
  float presure = P.pressure(Pressure::PASCALS);
  Serial.print("Presure = ");
  Serial.print(presure);
  Serial.println("P");

  /// Temperature reading
  // Instantiate temperature parser
  Temperature T;
  // Read temperatire
  OBC.MPL.readTemperature(&T);
  // Parse altitude with selected unit
  float temperature = T.temperature(Temperature::CELSIUS);
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.println("----------");
  delay(1000);
}