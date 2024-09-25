#ifndef GxRMC_H_
#define GxRMC_H_
#include <Arduino.h> 

#define RMC_FIELD_NUMBER (16)

class GxRMC_parser
{

	public:
	
		enum GxRMC_fieldPosEnum{IDENTIFIER_POS = 0, FIX_POS = 1, ALERT_POS = 2,
						LATITUDE_POS = 3, LATITUDE_NS_POS = 4, LONGITUDE_POS = 5,LONGITUDE_EW_POS = 6, 
						SPEED_POS = 7, CAP_POS = 8, date_FIX_POS = 9, MAGNETIC_POS = 10, MAGNETIC_EW_POS = 11};
							
		GxRMC_parser();
		GxRMC_parser(char* data);
		
		bool parse(char* data);
		void debugPrint(void);
		bool filedIsPresent(GxRMC_fieldPosEnum fieldId){return fieldPresence[fieldId];}
		
		float fix;
		char alert;
		float latitude;
		char ns;
		float longitude;
		char ew;
		float speed; // NOEUD
		float cap;
		uint32_t dateFix;
		float magnetic;
		char magneticEw;
		bool frameIsValid;
		uint32_t fieldPresenceNumber;
		
	protected:
		bool fieldPresence[RMC_FIELD_NUMBER];
		void extractField(char *field, GxRMC_fieldPosEnum fieldId);
		void clear(void);
};



#endif