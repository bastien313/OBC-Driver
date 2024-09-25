#include "checksum.h"
#include "GxGSV.h"


GxGSV_parser::GxGSV_parser(){
	clear();
}

GxGSV_parser::GxGSV_parser(char* data){
	clear();
	parse(data);
}

void GxGSV_parser::clear(void){
	fieldPresenceNumber = 0;
	totalMessage = 0;
	messageId = 0;
	svVisible = 0;

	for(unsigned int i = 0; i<4; i++){
		sv[i].prnNumber = 0;
		sv[i].elevation = 0;
		sv[i].azimuth = 0;
		sv[i].snr = 0;
		sv[i].dataIsPresent = false;
	}
	
	frameIsValid = false;
	for(unsigned int i = 0; i<GSV_FIELD_NUMBER; i++){
		fieldPresence[i] = false;
	}
}


bool GxGSV_parser::parse(char* frame){
	char field[16] = {0};
	uint32_t carFieldId = 0;
	uint32_t carFrameId = 0;
	uint32_t fieldId = 0;
	//Check if frame is valid.
	frameIsValid = isChecksumOk(frame);
	if(!frameIsValid){
		return frameIsValid;
	}
	
	// remove checksum
	frame[strlen(frame)-3] = 0;
	
	while(frame[carFrameId]){
		field[carFieldId] = frame[carFrameId];
		if(field[carFieldId] == ',' || field[carFieldId] == '\r'|| field[carFieldId] == '\n' || field[carFieldId] == '*'){
			field[carFieldId] = 0;
			if(strlen(field)){
				fieldPresenceNumber++;
				extractField(field, (GxGSV_fieldPosEnum)fieldId);
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




void GxGSV_parser::extractField(char *field, GxGSV_fieldPosEnum fieldId){

	if((int)fieldId > (int)SV_VISIBLE_POS  && (int)fieldId < (int)CHECKSUM_POS){
		uint32_t idSv = 0;
		for(int i = 3; i>=0; i--){
			if(fieldId > (int)SV_VISIBLE_POS + (i*4)){
				idSv = i;
				break;
			}
		}
		uint32_t svField = (int)fieldId - ((int)SV_VISIBLE_POS +1) - (idSv*4);
		switch(svField){
			case 0:
				sv[idSv].prnNumber = atoi(field);
				sv[idSv].dataIsPresent = true;
				break;
			case 1:
				sv[idSv].elevation = atoi(field);
				sv[idSv].dataIsPresent = true;
				break;
			case 2:
				sv[idSv].azimuth = atoi(field);
				sv[idSv].dataIsPresent = true;
				break;
			case 3:
				sv[idSv].snr = atoi(field);
				sv[idSv].dataIsPresent = true;
				break;
			default:
				break;	
		}
	}
	else{
		switch(fieldId){
		case TOTAL_MESSAGE_POS:
			totalMessage =  atoi(field);
			break;
			
		case MESSAGE_NUMBER_POS:
			messageId = atoi(field);
			break;
			
		case SV_VISIBLE_POS:
			svVisible = atoi(field);
			break;
			
		default:
			break;	
		}
	}
}

void GxGSV_parser::debugPrint(void){
	Serial.print("Total message = ");
	Serial.println(totalMessage);
	
	Serial.print("Messag id = ");
	Serial.println(messageId);

	Serial.print("svVisible = ");
	Serial.println(svVisible);
	
	Serial.print("FrameIsValid = ");
	Serial.println(frameIsValid);
	
	for(unsigned int i = 0; i<4; i++){
		if(sv[i].dataIsPresent){
			Serial.print("Sv ");
			Serial.print(i);
			Serial.println(" :");
			
			Serial.print("Prn =  ");
			Serial.println(sv[i].prnNumber);
			
			Serial.print("Elevation =  ");
			Serial.println(sv[i].elevation);
			
			Serial.print("Azimuth =  ");
			Serial.println(sv[i].azimuth);
			
			Serial.print("SNR =  ");
			Serial.println(sv[i].snr);
		}
	}


}