#include "checksum.h"

uint8_t calcChecksum(char* data, uint8_t size)
{
    uint8_t sum = 0;
    for (uint8_t i=0;i<size;i++)
    {   
		if(data[i] == '*'){
			break;
		}
		if(data[i] != '$'){
			sum ^= data[i];
		}
    }
    return sum;
}


uint8_t extractChecksum(char *nmea_str)
{
  char checksum_str[3] = {0};
  /* Find the start of the checksum message */
  char const * start_checksum = strchr(nmea_str, '*');
  /* Extract checksum. */
  strncpy(checksum_str, start_checksum + 1, 2);
  /* Convert to integer. */
  return strtol(checksum_str, nullptr, 16);
}


bool isChecksumOk(char *nmea_str)
{
  return (calcChecksum(nmea_str, strlen(nmea_str)) == extractChecksum(nmea_str));
}

/**************************************************************************************
 * INTERNAL FUNCTION DEFINITION
 **************************************************************************************/

