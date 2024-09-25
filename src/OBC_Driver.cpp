#include <OBC_Driver.h>
#include <Wire.h>

OBC_Driver::OBC_Driver(){

}

void OBC_Driver::begin(TwoWire* i2cInterface, UART* serial){
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
	GNSS.setNMEAoutputChannel((uint8_t*)"GGA", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GLL", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"RMC", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"VTG", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GSV", INISAT_GNSS::DDC);
	GNSS.setNMEAoutputChannel((uint8_t*)"GSA", INISAT_GNSS::DDC);
	
	m_serial = serial;
	m_serial->begin(115200);
}

float OBC_Driver::XP_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)0));
}

float OBC_Driver::XN_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)1));
}

float OBC_Driver::YP_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)2));
}

float OBC_Driver::YN_readVolatge(void){
	return m_sloarADC_XY.ADC_Conversion(m_sloarADC_XY.readADC_SingleEnded((uint8_t)3));
}

float OBC_Driver::Z_readVolatge(void){
	return m_sloarADC_Z.ADC_Conversion(m_sloarADC_Z.readADC_SingleEnded((uint8_t)0));
}

float OBC_Driver::thermistor_readVolatge(uint8_t channel){
	return m_thermistorADC.ADC_Conversion(m_thermistorADC.readADC_SingleEnded(channel));
}



/*********************************************************************************/
/*!
    @brief  Conversion function based on Steinhart-Hart Law.
            Coefficients must be changed to match the component used, the bias tension and resistor. 
            Currently on B57861S103F40, 5V, 10kOhms resistor.
*/
/*********************************************************************************/

float OBC_Driver::thermistor_voltageConversion(float Vtemp)
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

void OBC_Driver::hardwareReset_BNO(bool state){
	if(state){
		digitalWrite(RST_BNO_GPIO, LOW);  
	}else{
		digitalWrite(RST_BNO_GPIO, HIGH);  
	}
}

/*
* Read x byte from serial port(connected to COM board).
* Return amount of byte readed.
*/
uint32_t OBC_Driver::readCom(char *buff, uint32_t size){
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

/*
* Write x byte from serial port(connected to COM board).
*/
uint32_t OBC_Driver::writeCom(char *str){
	writeCom(str, strlen(str));
}

/*
* Write x byte from serial port(connected to COM board).
*/
void OBC_Driver::writeCom(char *buff, uint32_t len){
	for(uint32_t i = 0; i<len; i++){
		m_serial->write(*buff);
		*buff++;
	}
}