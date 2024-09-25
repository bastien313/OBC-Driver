#ifndef GxGSA_H_
#define GxGSA_H_
#include <Arduino.h> 

#define GSA_FIELD_NUMBER (20)

class GxGSA_parser
{
	enum GxGSA_fieldPosEnum{IDENTIFIER_POS = 0, MODE_POS = 1, FIX_TYPE_POS = 2, PRN_ID_POS = 3, 
							PDOP_POS = 15, HDOP_POS = 16, VDOP_POS = 17};
							
	public:
		GxGSA_parser();
		GxGSA_parser(char* data);
		
		bool parse(char* data);
		void debugPrint(void);
		bool filedIsPresent(GxGSA_fieldPosEnum fieldId){return fieldPresence[fieldId];}
		

		char mode;
		uint32_t fixType;
		uint32_t prnId[12];
		uint32_t prnNumber;
		float pdop;
		float hdop;
		float vdop;
		bool frameIsValid;
		uint32_t fieldPresenceNumber;

		
	protected:
		bool fieldPresence[GSA_FIELD_NUMBER];
		void extractField(char *field, GxGSA_fieldPosEnum fieldId);
		void clear(void);
};



#endif