/// @example GNSS.ino
#include <OBC_Arduino.h>
#include <string.h>

OBC_Arduino OBC;

void setup() {
  //Debuging use.
  Serial.begin(); 

  //Initialise OBC library
  OBC.begin(); 
}



void getMultipleFrame(void){

  // Read NMEA frame from device
  OBC.GNSS.fill_NMEA_buffer();

  //Print received frame RAW.
  Serial.println((char*)OBC.GNSS.NMEA_Buffer);

  // To obtain a GLL frame, we instantiate a GLL parser.
  GxGLL_parser GLL;
  // To obtain a RMC frame, we instantiate a RMC parser.
  GxRMC_parser RMC;

  // Disable device reading
  // The parser will use the most complete GGA frame
  if(OBC.GNSS.getGLLdata(&GLL, false)){ 
    // If a GLL frame is detected, we use the data.
    // The parser extracts data from the frame, but sometimes computations are needed for future use.
    int hTime, mTime, sTime;
    float latitude, longitude;

    latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GLL.latitude, GLL.ns);
    longitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GLL.longitude, GLL.ew);
    hTime = INISAT_GNSS::calcHour(GLL.fix);
    mTime = INISAT_GNSS::calcMinute(GLL.fix);
    sTime = INISAT_GNSS::calcSecond(GLL.fix);

    Serial.print("GLL Latitude = ");
    // Check if latitude is present in frame.
    if(GLL.filedIsPresent(GxGLL_parser::LATITUDE_POS)){
      Serial.println(latitude);
    }else{
      Serial.println("No data");
    }
    
    Serial.print("GLL Longitude = ");
    // Check if longitude is present in frame.
    if(GLL.filedIsPresent(GxGLL_parser::LONGITUDE_POS)){
      Serial.println(longitude);
    }else{
      Serial.println("No data");
    }

    Serial.print("GLL Time = ");
    Serial.print(hTime);
    Serial.print(":");
    Serial.print(mTime);
    Serial.print(":");
    Serial.println(sTime);
    Serial.println("----------");
  }else{
    Serial.println("No GLL frame present");
    Serial.println("----------");
  }

  // Disable device reading
  // The parser will use the most complete RMC frame
  if(OBC.GNSS.getRMCdata(&RMC, false)){ 
    // If a RMC frame is detected, we use the data.
    // The parser extracts data from the frame, but sometimes computations are needed for future use.
    int hTime, mTime, sTime;
    float latitude, longitude;

    latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(RMC.latitude, RMC.ns);
    longitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(RMC.longitude, RMC.ew);
    hTime = INISAT_GNSS::calcHour(RMC.fix);
    mTime = INISAT_GNSS::calcMinute(RMC.fix);
    sTime = INISAT_GNSS::calcSecond(RMC.fix);

    Serial.print("RMC Latitude = ");
    // Check if latitude is present in frame.
    if(RMC.filedIsPresent(GxRMC_parser::LATITUDE_POS)){
      Serial.println(latitude);
    }else{
      Serial.println("No data");
    }
    
    Serial.print("RMC Longitude = ");
    // Check if longitude is present in frame.
    if(RMC.filedIsPresent(GxRMC_parser::LONGITUDE_POS)){
      Serial.println(longitude);
    }else{
      Serial.println("No data");
    }

    Serial.print("RMC Time = ");
    Serial.print(hTime);
    Serial.print(":");
    Serial.print(mTime);
    Serial.print(":");
    Serial.println(sTime);
    Serial.println("----------");
  }else{
    Serial.println("No RMC frame present");
    Serial.println("----------");
  }


}

void getGGAFrame(void){
  // To obtain a GGA frame, we instantiate a GGA parser.
  GxGGA_parser GGA;
  // Read from device is enable with second parameter.
  // The parser will use the most compleete GGA frame
  if(OBC.GNSS.getGGAdata(&GGA, true)){ 
    // If a GGA frame is detected, we use the data.
    // The parser extracts data from the frame, but sometimes computations are needed for future use.
    int hTime, mTime, sTime;
    float latitude, longitude;

    latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GGA.latitude, GGA.ns);
    longitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GGA.longitude, GGA.ew);
    hTime = INISAT_GNSS::calcHour(GGA.fix);
    mTime = INISAT_GNSS::calcMinute(GGA.fix);
    sTime = INISAT_GNSS::calcSecond(GGA.fix);

    Serial.print("GGA Latitude = ");
    // Check if latitude is present in frame.
    if(GGA.filedIsPresent(GxGGA_parser::LATITUDE_POS)){
      Serial.println(latitude);
    }else{
      Serial.println("No data");
    }
    
    Serial.print("GGA Longitude = ");
    // Check if longitude is present in frame.
    if(GGA.filedIsPresent(GxGGA_parser::LONGITUDE_POS)){
      Serial.println(longitude);
    }else{
      Serial.println("No data");
    }

    Serial.print("GGA Time = ");
    Serial.print(hTime);
    Serial.print(":");
    Serial.print(mTime);
    Serial.print(":");
    Serial.println(sTime);
    Serial.println("----------");
  }else{
    Serial.println("No GGA frame present");
    Serial.println("----------");
  }
  
}

void loop() {

  getGGAFrame();
  getMultipleFrame();
  Serial.println(" ");
}