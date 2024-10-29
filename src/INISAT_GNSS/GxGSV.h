#ifndef GxGSV_H_
#define GxGSV_H_
#include <Arduino.h> 

#define GSV_FIELD_NUMBER (22)

typedef struct {
	uint32_t prnNumber;
	uint32_t elevation;
	uint32_t azimuth;
	uint32_t snr;
	bool dataIsPresent;
}GSV_SV;

class GxGSV_parser
{
	
							
	public:
	
		enum GxGSV_fieldPosEnum{IDENTIFIER_POS = 0, TOTAL_MESSAGE_POS = 1, MESSAGE_NUMBER_POS = 2, SV_VISIBLE_POS = 3, CHECKSUM_POS = 20};
		
		GxGSV_parser();
		GxGSV_parser(char* data);
		
		bool parse(char* data);
		void debugPrint(void);
		bool filedIsPresent(GxGSV_fieldPosEnum fieldId){return fieldPresence[fieldId];}
		

		uint32_t totalMessage;
		uint32_t messageId;
		uint32_t svVisible;
		GSV_SV sv[4];
		bool frameIsValid;
		uint32_t fieldPresenceNumber;

		
	protected:
		bool fieldPresence[GSV_FIELD_NUMBER];
		void extractField(char *field, GxGSV_fieldPosEnum fieldId);
		void clear(void);
};



#endif