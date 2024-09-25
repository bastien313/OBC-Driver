#ifndef OBC_DRIVER_H_
#define OBC_DRIVER_H_

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

class OBC_Driver
{
	public:
		OBC_Driver();
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
		uint32_t writeCom(char *str);
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