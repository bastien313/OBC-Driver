#include "INISAT_EPS.h"

#include <cstring>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

/// @brief Instantiates a new INISAT_EPS class
INISAT_EPS::INISAT_EPS()
{
}

/// @brief Initialise class.
///        Set I2C interface, I2C address, and initialise device.
/// @param i2c I2C interface, must be previously intitialised.
/// @param i2cAddress I2C device Address
void INISAT_EPS::begin(TwoWire* i2c, uint8_t i2cAddress){
    m_i2c = i2c;
	charger_addr = i2cAddress;
	init_ltc();
}

/// @brief Device initialsiation.
void  INISAT_EPS::init_ltc(){
	write_SMBUS(0x14, 0x00, 0x04); //On active la télémétrie même lorsqu'il n'y a pas de chargeur branché

    // Configuration des températures pour JEITA
    // Cas de la thermistance B=3200 des batteries 103456A-1S-3M
    // RQ: Pb de layout ici aussi: ces registres sont remis à zéro au début de la charge, à tester avec un PCB ayant un layout correct
    write_SMBUS(0x1F,0x3E,0x0B); //JEITA_T1
    write_SMBUS(0x20,0x36, 0x71); //JEITA_T2
    write_SMBUS(0x21,0x1F, 0xE9); //JEITA_T3
    write_SMBUS(0x22,0x1C, 0xC6); //JEITA_T4
    write_SMBUS(0x23,0x19, 0xE3); //JEITA_T5
    write_SMBUS(0x24,0x14, 0xDB); //JEITA_T6
}

/// @brief Return state of the device.
///        Member charger_state must be updated before call this.
/// @return 1 = fault, 2 = battery detection, 3 = charger_suspended, 4 = precharge, 5 = cc_cv_charge
///         6 = ntc_pause, 7 = timer_term, 8 = c over x term, 9 = max charge time fault, 10 = bat missing fault
///         11 = bat short fault, 0 = unknown
int INISAT_EPS::charger_state_display()
{
    int i;
    //Permet d'afficher l'état du LTC4162-L
    //printf("Statut du LTC4162-L: ");
    switch (charger_state){
        case 4096: i = 1;
            //printf("bat_detect_failed_fault");
            break;
        case 2048: i = 2;
            //printf("battery_detection");
            break;
        case 256: i = 3;
            //printf("charger_suspended");
            break;
        case 128: i = 4;
            //printf("precharge");
            break;
        case 64: i = 5;
            //printf("cc_cv_charge");
            break;
        case 32: i = 6;
            //printf("ntc_pause");
            break;
        case 16: i = 7;
            //printf("timer_term");
            break;
        case 8: i = 8;
            //printf("c_over_x_term");
            break;
        case 4: i = 9;
            //printf("max_charge_time_fault");
            break;
        case 2: i = 10;
            //printf("bat_missing_fault");
            break;
        case 1: i = 11;
            //printf("bat_short_fault");
            break;
        default: i = 0;
            //printf("unknown");
            break;        
        }
    return i;
}

/*===========================================================================================================================*/
/*========================================================== Write SMBUS ====================================================*/
/*===========================================================================================================================*/


/// @brief Writes 16-bits to the specified destination register
/// @param addr Address register
/// @param MSB 8 Most Significant Bit
/// @param LSB 8 Least  Significant Bit
void INISAT_EPS::write_SMBUS(int addr, int MSB, int LSB)
{   
    // Accès en écriture à un registre (16 bits) du LTC4162-L
	m_i2c->beginTransmission(charger_addr);
    char cmd[3];
    cmd[0]=addr;
    cmd[1]=LSB;
    cmd[2]=MSB;
	m_i2c->write(cmd, 3);
	m_i2c->endTransmission();
}

/// @brief Read 16-bits from the specified destination register
/// @param addr Address register
int INISAT_EPS::read_SMBUS(int addr)
{
    // Accès en lecture à un registre (16 bits) du LTC4162-L    
	m_i2c->requestFrom(charger_addr,2, addr,1,true);
	if (m_i2c->available()) {
		uint8_t firstByte = m_i2c->read();
		uint8_t secondByte = m_i2c->read();

		//m_i2c->endTransmission();
		return (secondByte << 8) + firstByte;
	}
    return 0;
}


/// @brief Reads all relevant data from the device and stores them in the following members.
///         charger_state, vin, vout, ibat, iin, die_temp, temperature, csd
void INISAT_EPS::get()
{
    int16_t thermistor_voltage;
    // Configuration au démarrage
    init_ltc();

    //Lecture et calcul de la télémétrie
    charger_state = read_SMBUS(0x34);
    vbat = read_SMBUS(0x3A)*192.4e-6;
    vin = read_SMBUS(0x3B)*1.649e-3;
    vout = read_SMBUS(0x3C)*1.653e-3;
    ibat = read_SMBUS(0x3D)*1.466e-6/RSNSB;
    iin = read_SMBUS(0x3E)*1.466e-6/RSNSI;
    die_temp = read_SMBUS(0x3F)*0.0215-264.4;
    
    //Calcul NTC (température batterie)
    thermistor_voltage=read_SMBUS(0x40);
    float Rntc = 10000.*thermistor_voltage/(21829-thermistor_voltage);
    float Tinv= 3.354e-3+log(Rntc/10000)/beta;
    temperature=1/Tinv-273.15;
            
    csd = charger_state_display();

    if (previous_charger_state==256 && charger_state!=256){
        init_ltc();
        }
    previous_charger_state=charger_state;
}

