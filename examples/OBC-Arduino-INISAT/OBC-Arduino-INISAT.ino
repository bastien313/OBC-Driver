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
        OBC.writeCom("AOBC check : OK@");
      break;

      case 'B': // B  (PARMETRES BATTERIES)
        EPS_dataOut();
      break;

      case 'C': // C  (TEMPERATURE)
        OBC.BNO.get_temp();
        OBC.writeCom("C");
        dtostrf(OBC.BNO.temperature, 7, 3, strFloat);
        OBC.writeCom(strFloat);
        OBC.writeCom("@");
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
        OBC.writeCom("V2@");
      break;      

      case 'Z': // Z  FIN DE COMMUNICATION 
        OBC.writeCom("Z");
      break;     
    }
  }
}

void EPS_dataOut(void){
        char strFloat[20];
        OBC.EPS.get();

        OBC.writeCom("B");
        dtostrf(OBC.EPS.vbat, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.vin, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.vout, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.ibat, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.iin, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.temperature, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("#");
        dtostrf(OBC.EPS.csd, 7, 3, strFloat);
        OBC.writeCom(strFloat);

        OBC.writeCom("@");
}

void MLP_altitude(void){
  char strFloat[20];
  Altitude A;
  OBC.MPL.setModeAltimeter();
  OBC.MPL.readAltitude(&A);

  OBC.writeCom("D");
  dtostrf(A.altitude(), 7, 3, strFloat);
  OBC.writeCom(strFloat);
  OBC.writeCom("@");
}

void MLP_pression(void){
  char strFloat[20];
  Pressure P;
  OBC.MPL.setModeBarometer();
  OBC.MPL.readPressure(&P);

  OBC.writeCom("E");
  dtostrf(P.pressure(), 7, 3, strFloat);
  OBC.writeCom(strFloat);
  OBC.writeCom("@");
}

void BNO_euler(void){
  char strFloat[20];
  OBC.BNO.get_angles(); 

  OBC.writeCom("F");
  dtostrf(OBC.BNO.euler.roll, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.euler.pitch, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.euler.yaw, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void  BNO_quaternion(void){
  char strFloat[20];
  OBC.BNO.get_quat();

  OBC.writeCom("G");
  dtostrf(OBC.BNO.quat.w, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.quat.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.quat.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.quat.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void BNO_angularSpeed(void){
  char strFloat[20];
  OBC.BNO.get_gyro();

  OBC.writeCom("H");
  dtostrf(OBC.BNO.gyro.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.gyro.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.gyro.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void BNO_accel(void){
  char strFloat[20];
  OBC.BNO.get_accel();

  OBC.writeCom("I");
  dtostrf(OBC.BNO.accel.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.accel.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.accel.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void BNO_mag(void){
  char strFloat[20];
  OBC.BNO.get_mag();

  OBC.writeCom("J");
  dtostrf(OBC.BNO.mag.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.mag.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.mag.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void BNO_lia(void){
  char strFloat[20];
  OBC.BNO.get_lia();

  OBC.writeCom("K");
  dtostrf(OBC.BNO.lia.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.lia.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.lia.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void BNO_gravity(void){
  char strFloat[20];
  OBC.BNO.get_grv();

  OBC.writeCom("L");
  dtostrf(OBC.BNO.gravity.x, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.gravity.y, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.BNO.gravity.z, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void solar(void){
  char strFloat[20];
  OBC.writeCom("M");
  dtostrf(OBC.XP_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.XN_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.YP_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.YN_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("#");
  dtostrf(OBC.Z_readVolatge()*50.0f, 7, 3, strFloat);
  OBC.writeCom(strFloat);

  OBC.writeCom("@");
}

void NMEA_bufferWrite(void){
  OBC.GNSS.fill_NMEA_buffer();

  if(strlen((char*)OBC.GNSS.NMEA_Buffer) == 0){
    OBC.writeCom("QNo data@");
  }else{
    OBC.writeCom("O");
    OBC.writeCom((char*)OBC.GNSS.NMEA_Buffer);
    OBC.writeCom("@");
  }
}

void NEMA_dataWrite(void){
  char bufferOut[256];
  OBC.GNSS.fill_NMEA_buffer();

  if(strlen((char*)OBC.GNSS.NMEA_Buffer) == 0){
    OBC.writeCom("PNo data@");
    return;
  }

  //Serial.print((char*)OBC.GNSS.NMEA_Buffer);

  int hTime, mTime, sTime, latitude, longitude;
  GxGGA_parser GGA;
  if(OBC.GNSS.getGGAdata(&GGA, false)){
      latitude = INISAT_GNSS::calcLatitude(GGA.latitude, GGA.ns);
      longitude = INISAT_GNSS::calcLongitude(GGA.longitude, GGA.ew);
      hTime = INISAT_GNSS::calcHour(GGA.fix);
      mTime = INISAT_GNSS::calcMinut(GGA.fix);
      sTime = INISAT_GNSS::calcSecond(GGA.fix);

      //GGA.debugPrint();

      if(latitude != 0 && longitude != 0){ // Data is valid
        sprintf(bufferOut, "N%02d:%02d:%02d#%d%c#%d%c#%d@", hTime, mTime, sTime, latitude, GGA.ns, longitude, GGA.ew, GGA.satNumber);
        OBC.writeCom(bufferOut);
        return; // data valid we skip
      }
  }

  GxGLL_parser GLL;
  if(OBC.GNSS.getGLLdata(&GLL, false)){
    latitude = INISAT_GNSS::calcLatitude(GLL.latitude, GLL.ns);
    longitude = INISAT_GNSS::calcLongitude(GLL.longitude, GLL.ew);
    hTime = INISAT_GNSS::calcHour(GLL.fix);
    mTime = INISAT_GNSS::calcMinut(GLL.fix);
    sTime = INISAT_GNSS::calcSecond(GLL.fix);

    //GLL.debugPrint();

    if(latitude != 0 && longitude != 0){ // Data is valid
      sprintf(bufferOut, "N%02d:%02d:%02d#%d%c#%d%c#%c@", hTime, mTime, sTime, latitude, GLL.ns, longitude, GLL.ew, '/');
      OBC.writeCom(bufferOut);
      return; // data valid we skip
    }
  }
  // No good data available
  OBC.writeCom("PNo data@");
}

void temperature(void){
  char strFloat[20];
  float voltage;
  OBC.writeCom("R");
  voltage = OBC.thermistor_readVolatge(0);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom("Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom("#");
  voltage = OBC.thermistor_readVolatge(1);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom("Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom("#");
  voltage = OBC.thermistor_readVolatge(2);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom("Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom("#");
  voltage = OBC.thermistor_readVolatge(3);
  if(voltage < 0.3f || voltage > 5.1f){
    OBC.writeCom("Deconnecté");
  }else{
    dtostrf(OBC_Arduino::thermistor_voltageConversion(voltage), 7, 3, strFloat);
    OBC.writeCom(strFloat);
  }
  
  OBC.writeCom("@");
}
