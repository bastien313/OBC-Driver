#ifndef INISAT_EPS_H
#define INISAT_EPS_H

#include <wire.h>

#define CELL_COUNT 2
#define Qrated 2
#define RSNSB 68e-3
#define RSNSI 68e-3
#define beta 3200


/// @brief Class for operating Battery charger device (LTC4162)
/// The standard usage is to call the get() function and read all the received data using the member 
/// vbat, vin, vout, ibat, iin, die_temp, temperature;
class INISAT_EPS 
{ 
public:
    INISAT_EPS();
	void begin(TwoWire* i2c = &Wire, uint8_t i2cAddress = 0x68);
    void init_ltc();
    void get();

	float vbat, vin, vout, ibat, iin, die_temp, temperature;
	int csd;
	
	
private:
	TwoWire*  m_i2c;
	uint8_t charger_addr;
	int16_t charger_state, previous_charger_state;
	void write_SMBUS(int addr, int MSB, int LSB);
    int read_SMBUS(int addr);
	int charger_state_display();
};

#endif