#ifndef OBC_ARDUINO_H_
#define OBC_ARDUINO_H_

#include <Wire.h>
#include <Arduino.h>
#include "ADS1015/ADS1015.h"
#include "EPS/INISAT_EPS.h"
#include "BNO055/BNO055.h"
#include "MPL3115A2/MPL3115A2.h"
#include "INISAT_GNSS/INISAT_GNSS.h"
#include "SD/SD.h"

#define RST_BNO_GPIO (2)
#define SD_CS_GPIO (10)

/// @brief This class is the high-level entry point of the library.
///Its purpose is to:
///    - Perform the correct initialization of the entire 2U system.
///    - Simplify the management of analog converters (ADS1115).
///    - Simplify communication with the COM board.
///    - Provide access to auxiliary libraries through the following members:
///       - EPS, Step-Down Battery Charger.
///       - BNO, Inertial Measurement Unit.
///       - MPL, pressure sensor with altimetry.
///       - GNSS, GPS Galileo GLONASS BeiDou.
///       - SDCARD.
class OBC_Arduino
{
	public:
		OBC_Arduino();
		void begin(TwoWire* i2cInterface = &Wire, UART* serial = &Serial1); // Initialisaton.
		
		//ADC functions
		float XP_readVolatge(void);
		float XN_readVolatge(void);
		float YP_readVolatge(void);
		float YN_readVolatge(void);
		float Z_readVolatge(void);
		float thermistor_readVolatge(uint8_t channel);
		static float thermistor_voltageConversion(float Vtemp);
		
		//COM function
		uint32_t readCom(char *buff, uint32_t size);
		void writeCom(char *str);
		void writeCom(char *buff, uint32_t len);
		
		//Additional library interface.
		//To use these libraries, see corresponding .h files to find out the possibilities.
		INISAT_EPS EPS;
		BNO055 BNO;
		MPL3115A2 MPL;
		INISAT_GNSS GNSS;
		SDFileSystemClass SDCARD; //Standard Arduino SD library, see https://www.arduino.cc/reference/en/libraries/sd/
		
	private:
		//Local ADC included on board.
		ADS1115 m_sloarADC_XY;
		ADS1115 m_sloarADC_Z;
		//Extern ADC on sensor board.
		ADS1115 m_thermistorADC;	
		void hardwareReset_BNO(bool);
		UART *m_serial;
};


#endif