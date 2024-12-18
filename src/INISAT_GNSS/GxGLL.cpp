#include "checksum.h"
#include "GxGLL.h"



/// @brief Instantiates a new GxGLL_parser class
///			Set all member to default value.
GxGLL_parser::GxGLL_parser(){
	clear();
}

/// @brief Instantiates a new GxGLL_parser class
///		   Parse data from NMEA frame
/// @param data C-string of NMEA frame
GxGLL_parser::GxGLL_parser(char* data){
	clear();
	parse(data);
}

/// @brief Set all member to default value.
/// @param  
void GxGLL_parser::clear(void){
	fieldPresenceNumber = 0;
	latitude = 0.0f;
	ns = 'X';
	longitude = 0.0f;
	ew = 'X';
	fix = 0.0f;
	status = 'X';
	mode = 'X';
	frameIsValid = false;
	for(unsigned int i = 0; i<GLL_FIELD_NUMBER; i++){
		fieldPresence[i] = false;
	}
}

/// @brief Parse NMEA frame
/// @param frame C-string of NMEA frame
/// @return True if frame is valid
bool GxGLL_parser::parse(char* frame){
	char field[32] = {0};
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
				extractField(field, (GxGLL_fieldPosEnum)fieldId);
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
void GxGLL_parser::extractField(char *field, GxGLL_fieldPosEnum fieldId){
	switch(fieldId){
		case FIX_POS:
			fix = atof(field);
			break;
			
		case LATITUDE_POS:
			latitude = atof(field);
			break;
			
		case LATITUDE_NS_POS:
			ns = field[0];
			break;
			
		case LONGITUDE_POS:
			longitude = atof(field);
			break;
			
		case LONGITUDE_EW_POS:
			ew = field[0];
			break;
			
		case STATUS_INDICATOR:
			status = field[0];
			if(status == 'V'){
				frameIsValid = false;
			}
			break;
			
		case MODE_INDICATOR:
			mode = field[0];
			break;
			
		default:
			break;
	}
}

/// @brief Print all field, for debuging usage
/// @param  
void GxGLL_parser::debugPrint(void){
	Serial.print("Fix = ");
	Serial.println(fix);
	
	Serial.print("Latitude = ");
	Serial.print(latitude);
	Serial.println(ns);
	
	Serial.print("Longitude = ");
	Serial.print(longitude);
	Serial.println(ew);
	
	Serial.print("status = ");
	Serial.println(status);
	
	Serial.print("mode = ");
	Serial.println(mode);
	
	Serial.print("FrameIsValid = ");
	Serial.println(frameIsValid);
	
}