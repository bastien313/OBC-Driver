# MPL3115

The MPL3115A2 is a compact, piezoresistive, absolute pressure sensor with an I2C digital interface.
MPL3115A2 has a wide operating range of 20 kPa to 110 kPa, a range that covers all surface elevations
on earth. The MEMS is temperature compensated utilizing an on-chip temperature sensor. 

The sensor must be set to the corresponding mode to read the desired data using the functions OBC.MPL.setModeBarometer() and OBC.MPL.setModeAltimeter(). These functions result in a long delay to acquire a new value, so use them only when necessary.
To read data from the sensor, using a parser is necessary. The parsers can be configured with the desired unit.

**List of available parsers:**

- Température
- Pression
- Altimetre

**Pressure example:**

```c
// Instantiate pressure parser
Pressure P;
// Set barometer mode
OBC.MPL.setModeBarometer();
// Read pressure
OBC.MPL.readPressure(&P);

float presure = P.pressure(Pressure::PASCALS);
Serial.print("Presure = ");
Serial.print(presure);
Serial.println("P");
```

Related Classes:

- MPL3115A2
- Altitude
- Pressure
- Temperature

Related Examples:

- MLP3115A2.ino
