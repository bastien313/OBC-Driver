# NEO-M8N

The NEO-M8 series of concurrent GNSS modules is built on the high-performing u-blox M8 GNSS
engine in the industry-proven NEO form factor.

This sub-library allows reading and extracting NMEA frames.

## Parser usage

The standard usage is to use the parser corresponding to the desired frame type to extract the useful information. The read function associated with the parser allows you to:

- Retrieve NMEA frames from the device;  
  This reading can be slow, so it is possible to disable it to extract multiple types of frames more quickly.
- Extract data from the most complete frame of the desired type.

Frames may be incomplete or erroneous; the parsers include functions that verify the validity of the frame and the presence of data.

**List of available parsers:**

- GxGGA
- GxGLL
- GxGSA
- GxGSV
- GxRMC

**GGA example:**

```c
GxGGA_parser GGA;
// Read from device is enable with second parameter.
if(OBC.GNSS.getGGAdata(&GGA, true)){ 
    // If a GGA frame is detected, we use the data.
    float latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GGA.latitude, GGA.ns);
    Serial.print("GGA Latitude = ");
    if(GGA.filedIsPresent(GxGGA_parser::LATITUDE_POS)){
      Serial.println(latitude);
    }else{
      Serial.println("No data");
    }
}
```

## Direct usage

"If the parser is not needed, NMEA frames can be directly read by calling `OBC.GNSS.fill_NMEA_buffer()`. All frames can be retrieved in `OBC.GNSS.NMEA_Buffer`.

**GGA example:**

```c
// Read NMEA frame from device
OBC.GNSS.fill_NMEA_buffer();

//Print received frame RAW.
Serial.println((char*)OBC.GNSS.NMEA_Buffer);
```

Related Classes:

- INISAT_GNSS
- GxRMC_parser
- GxGSV_parser
- GxGSA_parser
- GxGLL_parser
- GxGGA_parser

Related Examples:

- GNSS.ino
