#ifndef GxGLL_H_
#define GxGLL_H_
#include <Arduino.h> 

#define GLL_FIELD_NUMBER (8)

class GxGLL_parser
{
	enum GxGLL_fieldPosEnum{IDENTIFIER_POS = 0, LATITUDE_POS = 1, LATITUDE_NS_POS = 2, LONGITUDE_POS = 3,
							LONGITUDE_EW_POS = 4, FIX_POS = 5, STATUS_INDICATOR = 6, MODE_INDICATOR = 7};
							
	public:
		GxGLL_parser();
		GxGLL_parser(char* data);
		
		bool parse(char* data);
		void debugPrint(void);
		bool filedIsPresent(GxGLL_fieldPosEnum fieldId){return fieldPresence[fieldId];}
		
		float latitude;
		char ns;
		float longitude;
		char ew;
		float fix;
		char status;
		char mode;
		bool frameIsValid;
		uint32_t fieldPresenceNumber;

		
	protected:
		bool fieldPresence[GLL_FIELD_NUMBER];
		void extractField(char *field, GxGLL_fieldPosEnum fieldId);
		void clear(void);
};



#endif