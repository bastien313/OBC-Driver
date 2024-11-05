#include "checksum.h"
#include "GxRMC.h"

/// @brief Instantiates a new GxRMC_parser class
///			Set all member to default value.
GxRMC_parser::GxRMC_parser(){
	clear();
}

/// @brief Instantiates a new GxRMC_parser class
///		   Parse data from NMEA frame
/// @param data C-string of NMEA frame
GxRMC_parser::GxRMC_parser(char* data){
	clear();
	parse(data);
}

/// @brief Set all member to default value.
/// @param  
void GxRMC_parser::clear(void){
	fieldPresenceNumber = 0;
	fix = 0.0f;
	alert = 'X';
	latitude = 0.0f;
	ns = 'X';
	longitude = 0.0f;
	ew = 'X';
	speed = 0.0f;
	cap = 0.0f;
	dateFix = 0;
	magnetic = 0.0f,
	magneticEw = 'X';
	
	frameIsValid = false;
	for(unsigned int i = 0; i<RMC_FIELD_NUMBER; i++){
		fieldPresence[i] = false;
	}
}

/// @brief Parse NMEA frame
/// @param frame C-string of NMEA frame
/// @return True if frame is valid
bool GxRMC_parser::parse(char* frame){
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
				extractField(field, (GxRMC_fieldPosEnum)fieldId);
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
void GxRMC_parser::extractField(char *field, GxRMC_fieldPosEnum fieldId){
	switch(fieldId){
		case FIX_POS:
			fix = atof(field);
			break;
			
		case ALERT_POS:
			alert = field[0];
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
			
		case SPEED_POS:
			speed = atof(field);
			break;
			
		case CAP_POS:
			cap = atof(field);
			break;
			
		case date_FIX_POS:
			dateFix = atoi(field);
			break;
			
		case MAGNETIC_POS:
			magnetic = atof(field);
			break;
			
		case MAGNETIC_EW_POS:
			magneticEw = field[0];
			break;
			
		default:
			break;
	}
}

/// @brief Print all field, for debuging usage
/// @param  
void GxRMC_parser::debugPrint(void){
	Serial.print("Fix = ");
	Serial.println(fix);
	
	Serial.print("Alert = ");
	Serial.println(alert);
	
	Serial.print("Latitude = ");
	Serial.print(latitude);
	Serial.println(ns);
	
	Serial.print("Longitude = ");
	Serial.print(longitude);
	Serial.println(ew);
	
	Serial.print("speed = ");
	Serial.println(speed);
	
	Serial.print("cap = ");
	Serial.println(cap);
	
	Serial.print("Date Fix = ");
	Serial.println(dateFix);
	
	Serial.print("Magnetic = ");
	Serial.print(magnetic);
	Serial.println(magneticEw);	
	
	Serial.print("FrameIsValid = ");
	Serial.println(frameIsValid);
	
}