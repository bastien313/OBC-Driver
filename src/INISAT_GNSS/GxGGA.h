#ifndef GxGGA_H_
#define GxGGA_H_
#include <Arduino.h> 

#define GGA_FIELD_NUMBER (16)

class GxGGA_parser
{

	public:
	
		enum GxGGA_fieldPosEnum{IDENTIFIER_POS = 0, FIX_POS = 1, LATITUDE_POS = 2, LATITUDE_NS_POS = 3, LONGITUDE_POS = 4,
							LONGITUDE_EW_POS = 5, FIX_QUALIFICATION_POS = 6, NB_SAT_POS = 7, DOP_POS = 8, ALTITUDE_POS = 9,
							UNIT_ALTITUDE_POS = 10, HEIGHT_CORR_POS = 11, UNIT_HEIGHT_POS = 12};
							
		GxGGA_parser();
		GxGGA_parser(char* data);
		
		bool parse(char* data);
		void debugPrint(void);
		bool filedIsPresent(GxGGA_fieldPosEnum fieldId){return fieldPresence[fieldId];}
		
		float fix;
		float latitude;
		char ns;
		float longitude;
		char ew;
		uint32_t fixQualification;
		uint32_t satNumber;
		float dop;
		float altitude; // in meter
		float heightCorrection;
		bool frameIsValid;
		uint32_t fieldPresenceNumber;
		
	protected:
		bool fieldPresence[GGA_FIELD_NUMBER];
		void extractField(char *field, GxGGA_fieldPosEnum fieldId);
		void clear(void);
};



#endif