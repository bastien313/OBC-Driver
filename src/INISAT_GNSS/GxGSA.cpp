#include "checksum.h"
#include "GxGSA.h"


/// @brief Instantiates a new GxGSA_parser class
///			Set all member to default value.
GxGSA_parser::GxGSA_parser(){
	clear();
}

/// @brief Instantiates a new GxGSA_parser class
///		   Parse data from NMEA frame
/// @param data C-string of NMEA frame
GxGSA_parser::GxGSA_parser(char* data){
	clear();
	parse(data);
}

/// @brief Set all member to default value.
/// @param  
void GxGSA_parser::clear(void){
	fieldPresenceNumber = 0;
	mode = 'X';
	fixType = 0;
	

	for(unsigned int i = 0; i<12; i++){
		prnId[i] = 0;
	}
	prnNumber = 0;
	pdop = 0.0f;
	hdop = 0.0f;
	vdop = 0.0f;
	
	frameIsValid = false;
	for(unsigned int i = 0; i<GSA_FIELD_NUMBER; i++){
		fieldPresence[i] = false;
	}
}

/// @brief Parse NMEA frame
/// @param frame C-string of NMEA frame
/// @return True if frame is valid
bool GxGSA_parser::parse(char* frame){
	char field[16] = {0};
	uint32_t carFieldId = 0;
	uint32_t carFrameId = 0;
	uint32_t fieldId = 0;
	//Check if frame is valid.
	frameIsValid = isChecksumOk(frame);
	if(!frameIsValid){
		return frameIsValid;
	}
	
	while(frame[carFrameId]){
		field[carFieldId] = frame[carFrameId];
		if(field[carFieldId] == ',' || field[carFieldId] == '\r'|| field[carFieldId] == '\n' || field[carFieldId] == '*'){
			field[carFieldId] = 0;
			if(strlen(field)){
				fieldPresenceNumber++;
				extractField(field, (GxGSA_fieldPosEnum)fieldId);
				fieldPresence[fieldId] = true;
			}
			
			carFieldId = 0;
			field[carFieldId] = 0;
			fieldId++;
		}else{
			carFieldId++;
		}
		carFrameId++;
	}

	return frameIsValid;
}


/// @brief Extract one field of NMEA frame
/// @param field C-string of field
/// @param fieldId id of field
void GxGSA_parser::extractField(char *field, GxGSA_fieldPosEnum fieldId){

	if((int)fieldId > (int)PRN_ID_POS  && (int)fieldId < (int)PDOP_POS){
		prnId[(int)fieldId - (int)PRN_ID_POS - 1] = atoi(field);
		prnNumber++;
	}
	else{
		switch(fieldId){
		case MODE_POS:
			mode =  field[0];
			break;
			
		case FIX_TYPE_POS:
			fixType = atoi(field);
			break;
			
		case PDOP_POS:
			pdop = atof(field);
			break;
			
		case HDOP_POS:
			hdop = atof(field);
			break;
			
		case VDOP_POS:
			vdop = atof(field);
			break;
			
		default:
			break;	
		}
	}
}

/// @brief Print all field, for debuging usage
/// @param  
void GxGSA_parser::debugPrint(void){
	Serial.print("mode = ");
	Serial.println(mode);
	
	Serial.print("Fix type = ");
	Serial.println(fixType);
	
	for(unsigned int i = 0; i< prnNumber; i++){
		Serial.print("PRN ");
		Serial.print(i);
		Serial.print("= ");
		Serial.println(prnId[i]);
	}
	
	Serial.print("pdop = ");
	Serial.println(pdop);
	
	Serial.print("hdop = ");
	Serial.println(hdop);
	
	Serial.print("vdop = ");
	Serial.println(vdop);
	

	
	Serial.print("FrameIsValid = ");
	Serial.println(frameIsValid);
}