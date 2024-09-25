#ifndef INISAT_EPS_H
#define INISAT_EPS_H

#include <wire.h>

#define CELL_COUNT 2
#define Qrated 2
#define RSNSB 68e-3
#define RSNSI 68e-3
#define beta 3200

/** Class for operating GNSS over I2C **/
class INISAT_EPS 
{ 
public:
    INISAT_EPS();
	void begin(TwoWire* i2c = &Wire, uint8_t i2cAddress = 0x68);
/*===================================================== Init EPS ==========================================================*/
    void init_ltc();
/*================================================= Charger state display =================================================*/
    int charger_state_display();
/*==================================================== Soc Init ===========================================================*/
    void soc_init(float voc);
/*==================================================== Calcul soc =========================================================*/
    void calcul_soc();
/*===================================================== Write SMBUS =======================================================*/
    void write_SMBUS(int addr, int MSB, int LSB);
/*===================================================== Read SMBUS ========================================================*/
    int read_SMBUS(int addr);
/*====================================================== get EPS ==========================================================*/
    void get();
	void test();

	float vbat, vin, vout, ibat, iin, die_temp, voc, temperature;
	int csd;
	
	
private:
	TwoWire*  m_i2c;
	uint8_t charger_addr;
	float soc;
	int16_t JEITA_T1, JEITA_T2, JEITA_T3, JEITA_T4, JEITA_T5, JEITA_T6;
	float periode_lecture = 5.0; //secondes
	int16_t charger_state, previous_charger_state, config_bit_reg, thermistor_voltage;
};

#endif