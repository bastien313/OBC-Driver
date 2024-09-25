#include "checksum.h"
#include "GxGGA.h"


GxGGA_parser::GxGGA_parser(){
	clear();
}

GxGGA_parser::GxGGA_parser(char* data){
	clear();
	parse(data);
}

void GxGGA_parser::clear(void){
	fieldPresenceNumber = 0;
	fix = 0.0f;
	latitude = 0.0f;
	ns = 'X';
	longitude = 0.0f;
	ew = 'X';
	fixQualification = 0;
	satNumber = 0;
	dop = 0.0f;
	altitude = 0.0f;
	heightCorrection = 0.0f;
	frameIsValid = false;
	for(unsigned int i = 0; i<GGA_FIELD_NUMBER; i++){
		fieldPresence[i] = false;
	}
}


bool GxGGA_parser::parse(char* frame){
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
				extractField(field, (GxGGA_fieldPosEnum)fieldId);
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




void GxGGA_parser::extractField(char *field, GxGGA_fieldPosEnum fieldId){
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
			
		case FIX_QUALIFICATION_POS:
			fixQualification = atoi(field);
			break;
			
		case NB_SAT_POS:
			satNumber = atoi(field);
			break;
			
		case DOP_POS:
			dop = atof(field);
			break;
			
		case ALTITUDE_POS:
			altitude = atof(field);
			break;
			
		case HEIGHT_CORR_POS:
			heightCorrection = atof(field);
			break;
			
		default:
			break;
	}
}

void GxGGA_parser::debugPrint(void){
	Serial.print("Fix = ");
	Serial.println(fix);
	
	Serial.print("Latitude = ");
	Serial.print(latitude);
	Serial.println(ns);
	
	Serial.print("Longitude = ");
	Serial.print(longitude);
	Serial.println(ew);
	
	Serial.print("fixQualification = ");
	Serial.println(fixQualification);
	
	Serial.print("satNumber = ");
	Serial.println(satNumber);
	
	Serial.print("dop = ");
	Serial.println(dop);
	
	Serial.print("Altitude = ");
	Serial.println(altitude);
	
	Serial.print("HeightCorrection = ");
	Serial.println(heightCorrection);
	
	Serial.print("FrameIsValid = ");
	Serial.println(frameIsValid);
	
}