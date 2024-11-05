#ifndef INISAT_GNSS_H
#define INISAT_GNSS_H

#include <Wire.h>
#include "GxGGA.h"
#include "GxGLL.h"
#include "GxGSA.h"
#include "GxGSV.h"
#include "GxRMC.h"

#define DATA_BUFFER_SIZE (2048)

/// @brief Class for operating GNSS over I2C 
class INISAT_GNSS 
{ 
public:

    enum ouputChannel {NONE = 0x00, DDC = 0x01, USART1 = 0x02, USART2 = 0x04, USB = 0x08, SPI = 0x10 };
	enum frameFormat {GGA , GLL , GSA, GSV , RMC};
	
    INISAT_GNSS();
	void begin(TwoWire *i2c);
	void setNMEAoutputChannel(uint8_t *NMEAidentifier, ouputChannel channel);

	int32_t fill_NMEA_buffer(uint32_t timeOutMs = 1100);
	uint32_t getGGAdata(GxGGA_parser *outputParser, bool readEnable=true, uint32_t timeOutMs = 1100);
	uint32_t getGLLdata(GxGLL_parser *outputParser, bool readEnable=true, uint32_t timeOutMs = 1100);
	uint32_t getGSAdata(GxGSA_parser *outputParser, bool readEnable=true, uint32_t timeOutMs = 1100);
	uint32_t getGSVdata(GxGSV_parser *outputParser, bool readEnable=true, uint32_t timeOutMs = 1100);
	uint32_t getRMCdata(GxRMC_parser *outputParser, bool readEnable=true, uint32_t timeOutMs = 1100);

	static int calcHour(float fix);
	static int calcMinute(float fix);
	static int calcSecond(float fix);
	static float DecimalDegreesToGpsCoordinate (float decimalCoord, char quadrant);
	
	uint8_t NMEA_Buffer[DATA_BUFFER_SIZE];
      
private:
	TwoWire *m_i2c;	
	const uint8_t uBlox_addr = 0x42;             // READ GNSS Address
	const uint8_t MSB_ByteNb_addr = 0xFD;           // Available bytes number MSB address
	const uint8_t DataStream_addr = 0xFF;           // Data address
	
	uint32_t i2cRead(uint8_t regAddr, uint8_t *data, uint32_t size);
	void i2cWrite(uint8_t *buffer, unsigned int size);
	
	int32_t get_byteNb(void);
	void sendNMEAFrame(uint8_t *address, uint8_t *value);

	template <typename T>
	uint32_t getExtractedData(T *outputParser, frameFormat filter, bool readEnable, uint32_t timeOutMs);
};



#endif