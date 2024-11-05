#include <OBC_Arduino.h>
#include <Wire.h>

/// @brief Instantiates a new OBC_Arduino class
OBC_Arduino::OBC_Arduino(){

}

/// @brief Initializes the entire system.
///		   GPIO, I2C, UART. 
///		   ADS1115 x3(Solar_XY, Solar_Z, Thermistor), BNO055, NEO-M8N, MPL3115A2, LTC4162.
/// @param i2cInterface I2C Interface.
/// @param serial UART Interface
void OBC_Arduino::begin(TwoWire* i2cInterface, UART* serial){
	//GPIO
	pinMode(RST_BNO_GPIO, OUTPUT); //Reset BNO
	
	//I2C
	i2cInterface->begin();
	
	//ADC
	m_sloarADC_XY.begin(i2cInterface, (uint8_t)ADS1015_ADDRESS_VDD); // 0x49
	m_sloarADC_Z.begin(i2cInterface, (uint8_t)ADS1015_ADDRESS_SCL); // 0x4B 
	m_thermistorADC.begin(i2cInterface, ADS1015_ADDRESS_GND); // 0x48
	
	//EPS
	EPS.begin(i2cInterface, 0x68); // 0x68
	
	//BNO
	hardwareReset_BNO(false);
	BNO.begin(i2cInterface);
	BNO.reset();
	BNO.setmode(OPERATION_MODE_CONFIG);
	delay(100);
	BNO.set_mapping(3);
	delay(100);
	BNO.setmode(OPERATION_MODE_NDOF);
	delay(100);
	
	//MPL
	MPL.begin(i2cInterface);
	
	//GPS
	GNSS.begin(i2cInterface);
	// Enable all NMEA frame.
	GNSS.setNMEAoutputChannel((uint8_t*)"GGA", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GLL", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"RMC", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"VTG", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GSV", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GSA", INISAT_GNSS::DDC);
	
	//UART
	m_serial = serial;
	m_serial->begin(115200);
}

/// @brief Read Solar X+ voltage.
/// @param  
/// @return Solar X+ voltage.
float OBC_Arduino::XP_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)0));
}

/// @brief Read Solar X- voltage.
/// @param  
/// @return Solar X- voltage.
float OBC_Arduino::XN_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)1));
}

/// @brief Read Solar Y+ voltage.
/// @param  
/// @return Solar Y+ voltage.
float OBC_Arduino::YP_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)2));
}

/// @brief Read Solar Y- voltage.
/// @param  
/// @return Solar Y- voltage.
float OBC_Arduino::YN_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)3));
}

/// @brief Read Solar Z+ voltage.
/// @param  
/// @return Solar Z+ voltage.
float OBC_Arduino::Z_readVolatge(void){
	return m_sloarADC_Z.ADC_Conversion(m_sloarADC_Z.readADC_SingleEnded((uint8_t)0));
}

/// @brief Read selected thermistor voltage
/// @param  channel Desired channel
/// @return Selected thermistor voltage
float OBC_Arduino::thermistor_readVolatge(uint8_t channel){
	return m_thermistorADC.ADC_Conversion(m_thermistorADC.readADC_SingleEnded(channel));
}

/// @brief Convert voltage to temperature in °C
///		   Conversion function based on Steinhart-Hart Law.
///        Coefficients must be changed to match the component used, the bias tension and resistor. 
///        Currently on B57861S103F40, 5V, 10kOhms resistor.
/// @param Vtemp Theristor voltage
/// @return Temperature in °C
float OBC_Arduino::thermistor_voltageConversion(float Vtemp)
{
	/* Parameters (!!! need to be updated if changements in the circuit) */
	float_t Vbias = 5.0;  // Biasing Tension ! unaccurate
	float_t Rbias = 10000; // Biasing Resistor


    float_t Thermistor_T = 0; // Temperature converted in Celsius degrees
    float_t R = 0; // Resistor Value

    // Parameters in Python Program
    float_t A = 1.125966e-3; 
    float_t B = 2.3457279e-4; 
    float_t C = 8.6463018e-8;

    R = Rbias*(Vbias/Vtemp-1); // Thermistor value calculation from voltage
    Thermistor_T = 1/(A+B*log(R)+C*pow(log(R),3))-273.15; // Steinhart-Hart law
    return Thermistor_T;
}

/// @brief Drive the RST GPIO pin of BNO055
/// @param state State of the reset, TRUE = device in reset mode, FALSE = device un run mode.
void OBC_Arduino::hardwareReset_BNO(bool state){
	if(state){
		digitalWrite(RST_BNO_GPIO, LOW);  
	}else{
		digitalWrite(RST_BNO_GPIO, HIGH);  
	}
}

/// @brief Read x byte from UART(connected to COM board).
///        Unblocking method, if ther is no more data to reed return immediatly.
/// @param buff Buffer address to store readed data
/// @param size Maximum size to read.
/// @return Return amount of byte readed.
uint32_t OBC_Arduino::readCom(char *buff, uint32_t size){
	uint32_t dataReaded = 0;
	for(uint32_t i = 0; i<size; i++){
		if(m_serial->available() > 0){
			*buff = m_serial->read();
			buff++;
			dataReaded++;
		}else{
			return dataReaded;
		}
	}
	return dataReaded;
}

/// @brief Write a C-string to UART (connected to COM board).
/// @param str C-string to write
void OBC_Arduino::writeCom(char *str){
	writeCom(str, strlen(str));
}

/// @brief Write x byte to UART (connected to COM board).
/// @param buff Buffer address where data is stored.
/// @param len Quantity of bytes to write.
void OBC_Arduino::writeCom(char *buff, uint32_t len){
	for(uint32_t i = 0; i<len; i++){
		m_serial->write(*buff);
		*buff++;
	}
}