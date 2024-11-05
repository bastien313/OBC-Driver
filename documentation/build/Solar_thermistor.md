# Solar / Thermistor

## Thermistor

To read the thermistor, call `OBC.thermistor_readVoltage();` and then use `OBC_Arduino::thermistor_voltageConversion();` to convert the voltage to °C.

**Example:**

```c
//Read voltage of first thermistor (0)
float voltage = OBC.thermistor_readVolatge(0);
float temperature = OBC_Arduino::thermistor_voltageConversion(voltage);

Serial.print("Temperature 1 = ");
Serial.print(temperature);
Serial.println("°C");
```

## Solar

To read the solar sensor, call the desired sensor method `OBC.xx_readVoltage();`.

XP, XN, YP, YN, Z is available.

**Example:**

```c
float xp = OBC.XP_readVolatge();
float xn = OBC.XN_readVolatge();

Serial.print("X+ = ");
Serial.print(xp);
Serial.println("V");

Serial.print("X- = ");
Serial.print(xn);
Serial.println("V");
```

Related Classes:

- OBC_Arduino

Related Examples:

- ADC_SOLAR_TEMPERATURE.ino
