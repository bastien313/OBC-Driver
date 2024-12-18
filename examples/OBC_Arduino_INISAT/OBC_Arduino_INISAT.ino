/// @example OBC_Arduino_INISAT.ino

#include <OBC_Arduino.h>
#include <string.h>

OBC_Arduino OBC;

void setup() {
  Serial.begin(); //Debuging use.

  OBC.begin(); //Initialise library
}

void loop() {
  char strFloat[20];
  char command = 0;

  if(OBC.readCom(&command,1)){
    switch(command){

      case 'A': // A check
        OBC.writeCom((char*)"AOBC check : OK@");
      break;

      case 'B': // B  (PARMETRES BATTERIES)
        EPS_dataOut();
      break;

      case 'C': // C  (TEMPERATURE)
        OBC.BNO.get_temp();
        OBC.writeCom((char*)"C");
        dtostrf(OBC.BNO.temperature, 7, 3, strFloat);
        OBC.writeCom(strFloat);
        OBC.writeCom((char*)"@");
      break;

      case 'D': // D  (ALTITUDE)
        MLP_altitude();
      break;

      case 'E': // E  (PRESSION)
        MLP_pression();
      break;

      case 'F': // F  (ANGLE EULER)
        BNO_euler();
      break;

      case 'G': //  G  (QUATERNION)
        BNO_quaternion();
      break;

      case 'H': //  H  (VITESSE ANGULAIRE)
        BNO_angularSpeed();
      break;

      case 'I': //  I  (ACCELERATION ANGULAIRE)
        BNO_accel();
      break;

      case 'J': //  J  (CHAMP MAGNETIQUE)
        BNO_mag();
      break;

      case 'K': //   K  (ACCELERATION LINEAIRE)
        BNO_lia();
      break;

      case 'L': //   L  (GRAVITE)
        BNO_gravity();
      break;

      case 'M': //   M  (LUMINOSITE)
        solar();
      break;

      case 'N': //   O  (GNSS EXTRACT)
        NEMA_dataWrite();
      break;

      case 'O': //   O  (GNSS RAW)
        NMEA_bufferWrite();
      break;

      case 'R': //   R  (TEMPERATURES EXTERIEURES - ADC)
        
      break;


      case 'V': //   V  (GESTION INTERNE)
        OBC.writeCom((char*)"V2@");
      break;      

      case 'Z': // Z  FIN DE COMMUNICATION 
        OBC.writeCom((char*)"Z");
      break;     
    }
  }
}

void EPS_dataOut(void){
  char strFloat[20];
  OBC.EPS.get();

  OBC.writeCom((char*)"B");
  dtostrf(OBC.EPS.vbat, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.vin, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.vout, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.ibat, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.iin, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.temperature, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.EPS.csd, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void MLP_altitude(void){
  char strFloat[20];
  Altitude A;
  OBC.MPL.setModeAltimeter();
  OBC.MPL.readAltitude(&A);

  OBC.writeCom((char*)"D");
  dtostrf(A.altitude(), 7, 3, strFloat);
  OBC.writeCom(strFloat);
  OBC.writeCom((char*)"@");
}

void MLP_pression(void){
  char strFloat[20];
  Pressure P;
  OBC.MPL.setModeBarometer();
  OBC.MPL.readPressure(&P);

  OBC.writeCom((char*)"E");
  dtostrf(P.pressure(), 7, 3, strFloat);
  OBC.writeCom(strFloat);
  OBC.writeCom((char*)"@");
}

void BNO_euler(void){
  char strFloat[20];
  OBC.BNO.get_angles(); 

  OBC.writeCom((char*)"F");
  dtostrf(OBC.BNO.euler.roll, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.euler.pitch, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.euler.yaw, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void  BNO_quaternion(void){
  char strFloat[20];
  OBC.BNO.get_quat();

  OBC.writeCom((char*)"G");
  dtostrf(OBC.BNO.quat.w, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.quat.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.quat.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.quat.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void BNO_angularSpeed(void){
  char strFloat[20];
  OBC.BNO.get_gyro();

  OBC.writeCom((char*)"H");
  dtostrf(OBC.BNO.gyro.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.gyro.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.gyro.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void BNO_accel(void){
  char strFloat[20];
  OBC.BNO.get_accel();

  OBC.writeCom((char*)"I");
  dtostrf(OBC.BNO.accel.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.accel.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.accel.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void BNO_mag(void){
  char strFloat[20];
  OBC.BNO.get_mag();

  OBC.writeCom((char*)"J");
  dtostrf(OBC.BNO.mag.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.mag.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.mag.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void BNO_lia(void){
  char strFloat[20];
  OBC.BNO.get_lia();

  OBC.writeCom((char*)"K");
  dtostrf(OBC.BNO.lia.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.lia.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.lia.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void BNO_gravity(void){
  char strFloat[20];
  OBC.BNO.get_grv();

  OBC.writeCom((char*)"L");
  dtostrf(OBC.BNO.gravity.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.gravity.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.BNO.gravity.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void solar(void){
  char strFloat[20];
  OBC.writeCom((char*)"M");
  dtostrf(OBC.XP_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.XN_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.YP_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.YN_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"#");
  dtostrf(OBC.Z_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom((char*)"@");
}

void NMEA_bufferWrite(void){
  OBC.GNSS.fill_NMEA_buffer();

  if(strlen((char*)OBC.GNSS.NMEA_Buffer) == 0){
    OBC.writeCom((char*)"QNo data@");
  }else{
    OBC.writeCom((char*)"O");
    OBC.writeCom((char*)OBC.GNSS.NMEA_Buffer);
    OBC.writeCom((char*)"@");
  }
}


void sendNMEAData(int hTime, int mTime, int sTime, float latitude, char ns, float longitude, char ew, int satNumber){
  char bufferOut[50];
  OBC.writeCom((char*)"N");

  sprintf(bufferOut, "%02d:%02d:%02d", hTime, mTime, sTime);
  OBC.writeCom(bufferOut);

  OBC.writeCom((char*)"#");
  dtostrf(latitude, 1, 3, bufferOut);
  OBC.writeCom(bufferOut);
  OBC.writeCom((char*)&ns,1);

  OBC.writeCom((char*)"#");
  dtostrf(longitude, 1, 3, bufferOut);
  OBC.writeCom(bufferOut);
  OBC.writeCom((char*)&ew,1);

  OBC.writeCom((char*)"#");
  if(satNumber){
    sprintf(bufferOut, "%d",satNumber);
    OBC.writeCom(bufferOut);
  }else{
    OBC.writeCom((char*)"/");
  }

  OBC.writeCom((char*)"@");

}

void NEMA_dataWrite(void){
  char bufferOut[256];
  OBC.GNSS.fill_NMEA_buffer();

  if(strlen((char*)OBC.GNSS.NMEA_Buffer) == 0){
    OBC.writeCom((char*)"PNo data@");
    return;
  }

  //Serial.print((char*)OBC.GNSS.NMEA_Buffer);

  int hTime, mTime, sTime;
  float latitude, longitude;
  GxGGA_parser GGA;
  if(OBC.GNSS.getGGAdata(&GGA, false)){
    latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GGA.latitude, GGA.ns);
    longitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GGA.longitude, GGA.ew);
    hTime = INISAT_GNSS::calcHour(GGA.fix);
    mTime = INISAT_GNSS::calcMinute(GGA.fix);
    sTime = INISAT_GNSS::calcSecond(GGA.fix);

    //GGA.debugPrint();

    if(GGA.filedIsPresent(GxGGA_parser::LATITUDE_POS) && GGA.filedIsPresent(GxGGA_parser::LONGITUDE_POS)){ // Data is valid
      sendNMEAData(hTime, mTime, sTime, latitude, GGA.ns, longitude, GGA.ew, GGA.satNumber);
      return; // data valid we skip
    }
  }

  GxGLL_parser GLL;
  if(OBC.GNSS.getGLLdata(&GLL, false)){
    latitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GLL.latitude, GLL.ns);
    longitude = INISAT_GNSS::DecimalDegreesToGpsCoordinate(GLL.longitude, GLL.ew);
    hTime = INISAT_GNSS::calcHour(GLL.fix);
    mTime = INISAT_GNSS::calcMinute(GLL.fix);
    sTime = INISAT_GNSS::calcSecond(GLL.fix);

    //GLL.debugPrint();

    if(GLL.filedIsPresent(GxGLL_parser::LATITUDE_POS) && GLL.filedIsPresent(GxGLL_parser::LONGITUDE_POS)){ // Data is valid
      sendNMEAData(hTime, mTime, sTime, latitude, GLL.ns, longitude, GLL.ew, 0);
      return; // data valid we skip
    }
  }
  // No good data available
  OBC.writeCom((char*)"PNo data@");
}



void temperature(void){
  char strFloat[20];
  float voltage;
  OBC.writeCom((char*)"R");
  voltage = OBC.thermistor_readVolatge(0);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom((char*)"Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom((char*)"#");
  voltage = OBC.thermistor_readVolatge(1);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom((char*)"Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom((char*)"#");
  voltage = OBC.thermistor_readVolatge(2);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom((char*)"Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom((char*)"#");
  voltage = OBC.thermistor_readVolatge(3);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom((char*)"Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom((char*)"@");
}
