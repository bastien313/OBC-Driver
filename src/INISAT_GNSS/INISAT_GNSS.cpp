#include <cstdio>
#include "INISAT_GNSS.h"


/// @brief Convert value to the corresponding caracter.
/// @param a value to convert
/// @return The corresponding caracter.
static char value_to_char(char a) {
    if (a >= 0 && a <= 9) {
        return '0' + a;
    } else if (a >= 10 && a <= 15) {
        return 'A' + (a - 10);
    }
    return '\0';
}

/// @brief Compute checksum for NMEA frame
/// @param data data in the frame.
/// @param size Size of frame
/// @return the value of checksum.
static uint8_t cs_calculator(uint8_t* data, uint32_t size)
{
    uint8_t sum = 0;
    for (uint32_t i=0;i<size;i++)
    {   
        sum ^= data[i];
    }
    return sum;
}




/// @brief Instantiates a new INISAT_GNSS class
INISAT_GNSS::INISAT_GNSS()
{
	NMEA_Buffer[0] = 0;
}

/// @brief Set I2C interface,
/// @param i2c I2C interface, must be previously intitialised.
void INISAT_GNSS::begin(TwoWire *i2c){
	m_i2c = i2c;
}

/// @brief Read multiple bytes from device.
/// @param regAddr Address register
/// @param data  Buffer address to store readed data
/// @param size Maximum size to read.
uint32_t INISAT_GNSS::i2cRead(uint8_t regAddr, uint8_t *data, uint32_t size)
{
	uint32_t id;
	m_i2c->requestFrom(uBlox_addr,size, regAddr,1,true);
	for(id = 0; id < size; id++){
		if(m_i2c->available()>0){
			data[id] = m_i2c->read();
		}else{
			break;
		}
	}
	m_i2c->endTransmission();
	return id;
}

/// @brief Writes multiple bytes to the device
/// @param buffer Buffer address where data is stored.
/// @param size Quantity of bytes to write.
void INISAT_GNSS::i2cWrite(uint8_t *buffer, unsigned int size)
{	
	m_i2c->beginTransmission(uBlox_addr);
    m_i2c->write(buffer, size);
	m_i2c->endTransmission();
}

/// @brief Send nmea frame to device
/// Put address and value on an nmea frame.
/// $<address><value>*<checksum>
/// This method add start'$' and checksum
/// @param address C-string to write on <address> field.
/// @param value C-string to write on <value> field
void INISAT_GNSS::sendNMEAFrame(uint8_t *address, uint8_t *value){
	NMEA_Buffer[0] = 0;
	
	strcat((char*)NMEA_Buffer , "$");
	strcat((char*)NMEA_Buffer , (const char*)address);
	strcat((char*)NMEA_Buffer , (const char*)value);
	strcat((char*)NMEA_Buffer , ",");
	
	uint8_t chkSum = cs_calculator(&NMEA_Buffer[1], strlen((const char*)&NMEA_Buffer[1]));
	strcat((char*)NMEA_Buffer , "*");
	char chkString[3] = {0};
	chkString[1] = value_to_char(chkSum & 0x0F);
	chkString[0] = value_to_char(((chkSum >> 4) & 0x0F));
	strcat((char*)NMEA_Buffer , chkString);
	strcat((char*)NMEA_Buffer , "\r\n");
	
	i2cWrite(NMEA_Buffer, strlen((const char*)NMEA_Buffer));
	NMEA_Buffer[0] = 0;
}

/// @brief Set/Get message rate configuration (s) to/from the receiver.
/// Send rate is relative to the event a message is registered on. For example, if
/// the rate of a navigation message is set to 2, the message is sent every second
/// navigation solution.
/// @param NMEAidentifier C-string event to be received. Ex: "GGA" or "GSA"
/// @param channel Channel were received Channel where the data will be sent. DDC(I2C), USART1, USART2, USB
void INISAT_GNSS::setNMEAoutputChannel(uint8_t *NMEAidentifier, ouputChannel channel){
	uint8_t msg[100] = {0};
	uint8_t channelEnable[3] = {',', '0', 0};
	
	//strcat(msg, "$PUBX,40,");
	strcat((char*)msg, ",40,");
	strcat((char*)msg, (const char*)NMEAidentifier);
	
	channelEnable[1] = (channel & DDC)? '1':'0';
	strcat((char*)msg, (const char*)channelEnable);
	channelEnable[1] = (channel & USART1)? '1':'0';
	strcat((char*)msg, (const char*)channelEnable);
	channelEnable[1] = (channel & USART2)? '1':'0';
	strcat((char*)msg, (const char*)channelEnable);
	channelEnable[1] = (channel & USB)? '1':'0';
	strcat((char*)msg, (const char*)channelEnable);
	channelEnable[1] = (channel & SPI)? '1':'0';
	strcat((char*)msg, (const char*)channelEnable);
	
	sendNMEAFrame((uint8_t*)"PUBX", msg);
}

/// @brief Return number of data available from device
/// @param  
/// @return Number of data available, return -1 if and I2C error occur.
int32_t INISAT_GNSS::get_byteNb(void)
{
	uint8_t readBuffer[2];
	uint32_t readCnt = i2cRead(MSB_ByteNb_addr, readBuffer, 2);
	if(readCnt == 2){
		return readBuffer[0] << 8 | readBuffer[1];
	}else{
		return -1;
	}
	
}

/// @brief Fill NMEA_Buffer with data available from device.
///        NMEA frame can be slow, we wait while a frame are present until timeout occur.
/// @param timeOutMs Timeout in milliseconds
/// @return Return the size of data received.
int32_t INISAT_GNSS::fill_NMEA_buffer(uint32_t timeOutMs)
{
	int32_t sizeAvailable = 0;
	unsigned long inputTime = millis();
	
	while(sizeAvailable == 0){
		delay(100);
		if( (millis() - inputTime) >  timeOutMs){
			NMEA_Buffer[0] = 0;
			return 0;
		}
		sizeAvailable = get_byteNb();
	}
	
	//Buffer overflow protection.
	if(sizeAvailable >= DATA_BUFFER_SIZE){
		sizeAvailable = DATA_BUFFER_SIZE-1;
	}
	
	if(sizeAvailable > 0){
		uint32_t byteRemaining = (uint32_t)sizeAvailable;
		uint8_t *buffPtr = NMEA_Buffer;
		while(byteRemaining){
			if(byteRemaining > 255){
				i2cRead(DataStream_addr, buffPtr, 255);
				byteRemaining -= 255;
				buffPtr += 255;
			}else{
				i2cRead(DataStream_addr, buffPtr, byteRemaining);
				byteRemaining -= byteRemaining;
			}
		}
		NMEA_Buffer[sizeAvailable] = 0;		
	}
	return sizeAvailable;
}

 
/// @brief  Write data of the most complete GGA frame in outputParser.
/// @param outputParser pointer of parser structure.
/// @param readEnable If set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
uint32_t INISAT_GNSS::getGGAdata(GxGGA_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GGA, readEnable, timeOutMs);
}

/// @brief  Write data of the most complete GLL frame in outputParser.
/// @param outputParser pointer of parser structure.
/// @param readEnable If set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
uint32_t INISAT_GNSS::getGLLdata(GxGLL_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GLL, readEnable, timeOutMs);
}

/// @brief  Write data of the most complete GSA frame in outputParser.
/// @param outputParser pointer of parser structure.
/// @param readEnable If set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
uint32_t INISAT_GNSS::getGSAdata(GxGSA_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GSA, readEnable, timeOutMs);
}

/// @brief  Write data of the most complete GSV frame in outputParser.
/// @param outputParser pointer of parser structure.
/// @param readEnable If set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
uint32_t INISAT_GNSS::getGSVdata(GxGSV_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GSV, readEnable, timeOutMs);
}

/// @brief  Write data of the most complete RMC frame in outputParser.
/// @param outputParser pointer of parser structure.
/// @param readEnable If set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
uint32_t INISAT_GNSS::getRMCdata(GxRMC_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, RMC, readEnable, timeOutMs);
}


/// @brief Parse NMEA frame stored in local buffer NMEA_Buffer.
///		   Parse only frame type according to outputParser type (GxGSV_parser, GxGGA_parser, GxGSA_parser, GxGLL_parser GxRMC_parser).
/// 	   Parsed date are write in outputParser, only the most complete frame will be write on outputParser.
/// @tparam T type of parser GxGSV_parser, GxGGA_parser, GxGSA_parser, GxGLL_parser, GxRMC_parser
/// @param outputParser pointer of parser structure.
/// @param filter Frame type filter, must match the selected parser. if T = GxGSV_parser filter must be GSV.
/// @param readEnable if set to TRUE, NMEA_Buffer will be updated before parsing.
/// @param timeOutMs Timeout in millisecond if read is enable.
/// @return Number of frame detected.
template <typename T>
uint32_t INISAT_GNSS::getExtractedData(T *outputParser, frameFormat filter,  bool readEnable, uint32_t timeOutMs){
	if(readEnable){
		fill_NMEA_buffer(timeOutMs);
	}
	
	int j = 0;
	int i = 0;
	uint32_t score = 0;
	uint32_t frameNb = 0;
	char parse_data[256];
	
	char filterStr[10];
	
	switch(filter){
		case GGA:
			strcpy(filterStr, "GGA");
			break;
		case GLL:
			strcpy(filterStr, "GLL");
			break;
		case GSA:
			strcpy(filterStr, "GSA");
			break;
		case GSV:
			strcpy(filterStr, "GSV");
			break;
		case RMC:
			strcpy(filterStr, "RMC");
			break;
	}
	
	while(NMEA_Buffer[i]){
		if(i >= DATA_BUFFER_SIZE){//Prevent buffer overflow
			break;
		}
		parse_data[j] = NMEA_Buffer[i];
		if (NMEA_Buffer[i]=='\n')  {
			parse_data[j] = 0;
			j = 0;
			if(strstr(parse_data, filterStr)){
				frameNb++;
				T parser(parse_data);
				if(parser.fieldPresenceNumber > score && parser.frameIsValid){
					score = parser.fieldPresenceNumber;
					outputParser->parse(parse_data);
				}
			}
		}else{
			j++;
		}
		i++;
	}
	return frameNb;
}


/// @brief Compute hour from fix
/// @param fix fix value
/// @return Hour
int INISAT_GNSS::calcHour(float fix){
	return int(fix) / 10000;
}

/// @brief Compute minute from fix
/// @param fix fix value
/// @return Minute
int INISAT_GNSS::calcMinute(float fix){
	return (int(fix) % 10000) / 100;
}

/// @brief Compute second from fix
/// @param fix fix value
/// @return Second
int INISAT_GNSS::calcSecond(float fix){
	return int(fix) % 100;
}

/// @brief Compute frame coordinate to GPS coordinate 
/// @param decimalCoord Coordinate from NMEA frame
/// @param quadrant Quadrant from NMEA frame ('N', 'S', 'E', 'W')
/// @return The GPS coordinate
float INISAT_GNSS::DecimalDegreesToGpsCoordinate (float decimalCoord, char quadrant){
	
	int decimalPart = (int)(decimalCoord/100.0f);
	float floatPart = decimalCoord - (float)(decimalPart)*100.0f;
	float degree = (float)decimalPart + floatPart/60.0f;

	if (quadrant == 'S' || quadrant == 'W')
		degree *= -1.0f;
	return degree;
}
