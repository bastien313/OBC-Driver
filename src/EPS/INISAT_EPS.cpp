#include "INISAT_EPS.h"

#include <cstring>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>


INISAT_EPS::INISAT_EPS()
{
}
/*===========================================================================================================================*/
/*======================================================= Init EPS ==========================================================*/
/*===========================================================================================================================*/
void INISAT_EPS::begin(TwoWire* i2c, uint8_t i2cAddress){
    m_i2c = i2c;
	charger_addr = i2cAddress;
	init_ltc();
}

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
/*===========================================================================================================================*/
/*=============================================== Charger state display =====================================================*/
/*===========================================================================================================================*/
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
/*======================================================== Soc Init =========================================================*/
/*===========================================================================================================================*/
void INISAT_EPS::soc_init(float voc)
{
    // Détermination de l'état de charge SOC au démarrage
        // Cette approche est précise si la batterie n'est ni en charge ni en décharge
        // Source: https://arxiv.org/ftp/arxiv/papers/1803/1803.10654.pdf
        // Remarque: les valeurs prises sont celles données dans le papier, il pourrait être interessant de les adapter à notre batterie.
        float A,B;
        if (voc>4.039){
            A=90.61;
            B=274.7;
        }
        else if (voc>3.967){
            A=104.8;
            B=332;
        }
        else if (voc>3.739){
            A=111.9;
            B=359.9;
        }
        else if (voc>3.676){
            A=229.5;
            B=800.9;
        }
        else if (voc>3.595){
            A=334;
            B=1225;
        }
        else if (voc>3.508){
            A=149;
            B=516.1;
        }
        else if (voc>3.452){
            A=125;
            B=431.1;
        }
        else if (voc>3.3){
            A=26.55;
            B=88.6;
        }
        else{
            //printf("La tension voc est trop basse pour estimer le SOC, recharger la batterie.\n");
            A=0;
            B=0;
            }
        soc=A*voc-B;
}

/*===========================================================================================================================*/
/*======================================================== Calcul soc =======================================================*/
/*===========================================================================================================================*/
void INISAT_EPS::calcul_soc()
{
   // Calcul de l'état de charge SOC
        // Il s'agit d'un compteur de Coulomb prenant en compte l'impact de la température sur la capacité de la batterie
        // Source: https://arxiv.org/ftp/arxiv/papers/1803/1803.10654.pdf
        
        float alpha;
        float delta_t=periode_lecture/3600; //en heures
        
        if (temperature<-10){
            alpha=0.5;
            }
        else if (temperature<5){
            alpha=0.6;
            }
        else if (temperature<25){
            alpha=0.8;
            }
        else if (temperature<45){
            alpha=1;
            }
        else{
            alpha=0.9;
            }
        if (charger_state==16){ //Si la charge est terminée (timer_term)
            soc=100; //on recalibre le SOC
            }
        else{
            soc=soc+(100*ibat*delta_t)/(Qrated*alpha); //ibat positif en charge, négatif en décharge
            }
}
/*===========================================================================================================================*/
/*========================================================== Write SMBUS ====================================================*/
/*===========================================================================================================================*/
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
/*===========================================================================================================================*/
/*========================================================= Read SMBUS ======================================================*/
/*===========================================================================================================================*/
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

void INISAT_EPS::test(){
	vin = read_SMBUS(0x3B)*1.649e-3;
}
/*===========================================================================================================================*/
/*========================================================== get EPS ========================================================*/
/*===========================================================================================================================*/
void INISAT_EPS::get()
{
    //printf("BATTERY MANAGEMENT MONITOR\n");
    
    // Configuration au démarrage
    init_ltc();
    
    //read_SMBUS(0x3A)*192.4E-6; //Mesure tension des batteries au demarrage
    soc_init(voc); //Calcul de l'état de charge au démarrage

    

        //Lecture et calcul de la télémétrie
        charger_state = read_SMBUS(0x34);
        config_bit_reg = read_SMBUS(0x14);
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
                
        // Mise à jour du SOC
        calcul_soc();
        csd = charger_state_display();
        // Si une alimentation est branchée, charger_state passe de 256 à une autre valeur
        // Lorsque c'est le cas, il faut reécrire les registres de configuration
        if (previous_charger_state==256 && charger_state!=256){
            init_ltc();
            }
        previous_charger_state=charger_state;
        /*string B = "B" + to_string(vbat) + "#" + to_string(vin) + "#" + to_string(vout) +  "#" + to_string(ibat) +  "#" + to_string(iin) +  "#" + to_string(temperature) +  "#" + to_string(csd) + "@";
        //printf("B%.3f#%.3f#%.3f#%.3f#%.3f#%.3f#%d@",vbat,vin,vout,ibat,iin,temperature,csd);
        COMM.write(B.c_str(),strlen(B.c_str()));*/
        //thread_sleep_for(10 * periode_lecture);
}

