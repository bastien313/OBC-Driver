#include <cstdio>
#include "INISAT_GNSS.h"


char value_to_char (char a)
{
    char r;
    switch (a) 
    {
        case 0 : a = '0';
            break;
        case 1 : a = '1';
            break;
        case 2 : a = '2';
            break;
        case 3 : a = '3';
            break;
        case 4 : a = '4';
            break;
        case 5 : a = '5';
            break;
        case 6 : a = '6';
            break;
        case 7 : a = '7';
            break;
        case 8 : a = '8';
            break;
        case 9 : a = '9';
            break;
        case 10 : a = 'A';
            break;
        case 11 : a = 'B';
            break;
        case 12 : a = 'C';
            break;
        case 13 : a = 'D';
            break;
        case 14 : a = 'E';
            break;
        case 15 : a = 'F';
            break;
    }
    r = a;
    return r;
}


INISAT_GNSS::INISAT_GNSS()
{
	NMEA_Buffer[0] = 0;
}

void INISAT_GNSS::begin(TwoWire *i2c){
	m_i2c = i2c;
}


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



 
void INISAT_GNSS::i2cWrite(uint8_t *buffer, unsigned int size)
{	
	m_i2c->beginTransmission(uBlox_addr);
    m_i2c->write(buffer, size);
	m_i2c->endTransmission();

}

/*===========================================================================================================================*/
/*================================================== Config GNSS ============================================================*/
/*===========================================================================================================================*/


/*
* Compute checksum and 
*/
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
	//Serial.println((char*)NMEA_Buffer);
	NMEA_Buffer[0] = 0;
}


/*
Set/Get message rate configuration (s) to/from the receiver.
• Send rate is relative to the event a message is registered on. For example, if
the rate of a navigation message is set to 2, the message is sent every second
navigation solution.
*/
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
/*===========================================================================================================================*/
/*=========================================== get byte number GNSS ==========================================================*/
/*===========================================================================================================================*/
/*
Return number of sta available.
If there is an error on I2C bus return -1.
*/
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

/*===========================================================================================================================*/
/*======================================================= get_NMEA ==========================================================*/
/*===========================================================================================================================*/

/*
* fill NMEA_Buffer.
* Return the size of data received.
* NMEA frame can be slow, we wait while a frame are present until timeout occur.
*/
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

/*===========================================================================================================================*/
/*============================================== checksum calculator GNSS ===================================================*/
/*===========================================================================================================================*/
uint8_t INISAT_GNSS::cs_calculator(uint8_t* data, uint32_t size)
{
    uint8_t sum = 0;
    for (uint32_t i=0;i<size;i++)
    {   
        sum ^= data[i];
    }
    return sum;
}


 
/*
* Write data of the most coplete GGA frame in outputParser.
* Return the number of GGA frame.
*/
uint32_t INISAT_GNSS::getGGAdata(GxGGA_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GGA, readEnable, timeOutMs);
}

/*
* Write data of the most coplete GLL frame in outputParser.
* Return the number of GLL frame.
* If readEnable is false, fill_NMEA_buffer() must be caled before.
*/
uint32_t INISAT_GNSS::getGLLdata(GxGLL_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GLL, readEnable, timeOutMs);
}

/*
* Write data of the most coplete GSA frame in outputParser.
* Return the number of GSA frame.
* If readEnable is false, fill_NMEA_buffer() must be caled before.
*/
uint32_t INISAT_GNSS::getGSAdata(GxGSA_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GSA, readEnable, timeOutMs);
}

/*
* Write data of the most coplete GSV frame in outputParser.
* Return the number of GSV frame.
* If readEnable is false, fill_NMEA_buffer() must be caled before.
*/
uint32_t INISAT_GNSS::getGSVdata(GxGSV_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, GSV, readEnable, timeOutMs);
}

/*
* Write data of the most coplete RMC frame in outputParser.
* Return the number of RMC frame.
* If readEnable is false, fill_NMEA_buffer() must be caled before.
*/
uint32_t INISAT_GNSS::getRMCdata(GxRMC_parser *outputParser, bool readEnable, uint32_t timeOutMs){
	return getExtractedData(outputParser, RMC, readEnable, timeOutMs);
}




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
				if(parser.fieldPresenceNumber > score){
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


/*===========================================================================================================================*/
/*================================================== NMEA time parser function ==============================================*/
/*===========================================================================================================================*/
int INISAT_GNSS::calcHour (float fix){
	return int(fix) / 10000;
}

int INISAT_GNSS::calcMinut (float fix){
	return (int(fix) % 10000) / 100;
}
int INISAT_GNSS::calcSecond (float fix){
	return int(fix) % 100;
}

int INISAT_GNSS::calcLatitude (float NMEA_lat, char ns){
	int latitude=int(NMEA_lat/100)+(NMEA_lat-int(NMEA_lat/100)*100)/60;
	if (ns == 'S')
		latitude *= -1;
	return latitude;
}

int INISAT_GNSS::calcLongitude (float NMEA_long, char ew){
	int longitude = int(NMEA_long/100)+(NMEA_long-int(NMEA_long/100)*100)/60;
	if (ew == 'W')
		longitude *= -1;
	return longitude;
}