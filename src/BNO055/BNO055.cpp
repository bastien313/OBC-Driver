#include "BNO055.h"
#include <Arduino.h>

/// @brief Instantiates a new BNO055 class
///        Initialise with default configuration value.
BNO055::BNO055(){
    address = BNOAddress;
    accel_scale = 1.0f;
    rate_scale = 1.0f/16.0f;
    angle_scale = 1.0f/16.0f;
    temp_scale = 1;
}

/// @brief Set I2C interface,
/// @param i2c I2C interface, must be previously intitialised.
void BNO055::begin(TwoWire*  i2c){
	m_i2c = i2c;
}

/// @brief Writes multiple bytes to the specified destination register
/// @param reg Address register
/// @param value Buffer address where data is stored.
/// @param size Quantity of bytes to write.
void BNO055::writeRegister(uint8_t reg, uint8_t *value, uint8_t size){
	m_i2c->beginTransmission(address);
	m_i2c->write(&reg, 1);
	m_i2c->write(value, size);
	m_i2c->endTransmission();
}

/// @brief Writes one bytes to the specified destination register
/// @param reg Address register
/// @param value Value to write
void BNO055::writeRegister(uint8_t reg, uint8_t value){
	writeRegister(reg, &value, 1);
}


/// @brief Read multiple bytes from device.
/// @param reg Address register
/// @param value  Buffer address to store readed data
/// @param size Maximum size to read.
void BNO055::readRegister(uint8_t reg, uint8_t *value, uint32_t size){
	for(unsigned int i = 0; i<size; i++){
		m_i2c->requestFrom(address,1, reg,1,true);
		reg++;
		value[i] = m_i2c->read();
	}
}


/// @brief Read one byte from device.
/// @param reg Address register
/// @return Return the value readed byte.
uint8_t BNO055::readRegister(uint8_t reg){
	uint8_t datareceive = 0;
	readRegister(reg, &datareceive, 1);
	return datareceive;
}

/// @brief Change page of memeory device
/// @param value Page ID
void BNO055::setpage(uint8_t value){
	writeRegister(BNO055_PAGE_ID_ADDR,value);
}

/// @brief Perform a power-on-reset(software)
void BNO055::reset(){
//Perform a power-on-reset
	setpage(0);
	uint8_t rx = readRegister(BNO055_SYS_TRIGGER_ADDR);

	writeRegister(BNO055_SYS_TRIGGER_ADDR, rx | 0x20);
	//Wait for the system to come back up again (datasheet says 650ms)
	delay(675);
}
    
/// @brief Check that the BNO055 is connected and download the software details 
//         and serial number of chip and store in ID structure
/// @return false if an error occured
bool BNO055::check(){
//Check we have communication link with the chip
	setpage(0);
	uint8_t rx = readRegister(BNO055_CHIP_ID_ADDR);
	uint8_t rawdata[7];
    if (rx != 0xA0) return false;
//Grab the chip ID and software versions
	readRegister(BNO055_CHIP_ID_ADDR, rawdata, 7);
    ID.id = rawdata[0];
    ID.accel = rawdata[1];
    ID.mag = rawdata[2];
    ID.gyro = rawdata[3];
    ID.sw[0] = rawdata[4];
    ID.sw[1] = rawdata[5];
    ID.bootload = rawdata[6];
    setpage(1);
	readRegister(BNO055_UNIQUE_ID_ADDR, (uint8_t*)ID.serial, 16);
    setpage(0);
    return true;
    }
    
/// @brief Turn the external timing crystal on/off
/// @param yn TRUE = on, FALSE = off
void BNO055::SetExternalCrystal(bool yn){
// Read the current status from the device
	setpage(0);
	uint8_t rx = readRegister(BNO055_SYS_TRIGGER_ADDR);
    if (yn) rx = rx | 0x80;
    else rx = rx & 0x7F;
	writeRegister(BNO055_SYS_TRIGGER_ADDR,rx);
}

/// @brief Set the operation mode of the sensor
/// @param omode Operating mode, see OPERATION_MODE_X define
void BNO055::setmode(char omode){
	setpage(0);
	writeRegister(BNO055_OPR_MODE_ADDR,omode);
    op_mode = omode;
}

/// @brief Set the power mode of the sensor
/// @param pmode power mode, see POWER_MODE_X define
void BNO055::setpowermode(char pmode){
	setpage(0);
	writeRegister(BNO055_PWR_MODE_ADDR,pmode);
    pwr_mode = pmode;
}

/// @brief Set the output units from the accelerometer
/// @param units MPERSPERS or MILLIG
void BNO055::set_accel_units(char units){
	setpage(0);
	uint8_t rx = readRegister(BNO055_UNIT_SEL_ADDR);
    if(units == MPERSPERS){
        rx = rx & 0xFE;
        accel_scale = 0.01f;
        }
    else {
        rx = rx | units;
        accel_scale = 1.0f;
        }
	writeRegister(BNO055_UNIT_SEL_ADDR,rx);
}

/// @brief Set the output units from the gyroscope
/// @param units DEG_PER_SEC or RAD_PER_SEC
void BNO055::set_anglerate_units(char units){
	setpage(0);
	uint8_t rx = readRegister(BNO055_UNIT_SEL_ADDR);
    if (units == DEG_PER_SEC){
        rx = rx & 0xFD;
        rate_scale = 1.0f/16.0f;
        }
    else {
        rx = rx | units;
        rate_scale = 1.0f/900.0f;
        }
    //writechar(BNO055_UNIT_SEL_ADDR,rx);
	writeRegister(BNO055_UNIT_SEL_ADDR,rx);
}    

/// @brief Set the output units from the IMU
/// @param units DEGREES or RADIANS
void BNO055::set_angle_units(char units){
	setpage(0);
	uint8_t rx = readRegister(BNO055_UNIT_SEL_ADDR);
    if (units == DEGREES){
        rx = rx & 0xFB;
        angle_scale = 1.0f/16.0f;
        }
    else {
        rx = rx | units;
        rate_scale = 1.0f/900.0f;
        }
	writeRegister(BNO055_UNIT_SEL_ADDR,rx);
}    

/// @brief Set the output units from the temperature sensor
/// @param units CENTIGRADE or FAHRENHEI
void BNO055::set_temp_units(char units){
	setpage(0);
	uint8_t rx = readRegister(BNO055_UNIT_SEL_ADDR);
    if (units == CENTIGRADE){
        rx = rx & 0xEF;
        temp_scale = 1;
        }
    else {
        rx = rx | units;
        temp_scale = 2;
        }
	writeRegister(BNO055_UNIT_SEL_ADDR,rx);
}    

/// @brief Set the data output format 
/// @param units WINDOWS or ANDROID
void BNO055::set_orientation(char units){
	setpage(0);
	uint8_t rx = readRegister(BNO055_UNIT_SEL_ADDR);
    if (units == WINDOWS) rx = rx &0x7F;
    else rx = rx | units;
	writeRegister(BNO055_UNIT_SEL_ADDR,rx);
}        

/// @brief Set the mapping of the exes/directions as per page 25 of datasheet
/// @param orient range 0-7, any value outside this will set the orientation to P1 (default at power up)
void BNO055::set_mapping(char orient){
	setpage(0);
    switch (orient){
        case 0: 
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x21);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x04);
            break;
        case 1:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x24);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x00);
            break;
        case 2:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x24);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x00);
            break;
        case 3:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x21);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x02);
            break;
        case 4:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x24);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x03);
            break;
        case 5:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x21);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x01);
            break;
        case 6:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x21);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x07);
            break;
        case 7:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x24);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x05);
            break;
        default:
			writeRegister(BNO055_AXIS_MAP_CONFIG_ADDR,0x24);
			writeRegister(BNO055_AXIS_MAP_SIGN_ADDR,0x00);
        }
}

/// @brief Get the current values from the accelerometer
///        Store the result in accel member.
/// @param  
void BNO055::get_accel(void){
	uint8_t rawdata[6];
	setpage(0);
	readRegister(BNO055_ACCEL_DATA_X_LSB_ADDR, rawdata, 6);
    accel.rawx = (rawdata[1] << 8 | rawdata[0]);
    accel.rawy = (rawdata[3] << 8 | rawdata[2]);
    accel.rawz = (rawdata[5] << 8 | rawdata[4]);
    accel.x = float(accel.rawx)*accel_scale;
    accel.y = float(accel.rawy)*accel_scale;
    accel.z = float(accel.rawz)*accel_scale;
}
    
/// @brief Get the current values from the gyroscope
///        Store the result in gyro member.
/// @param  
void BNO055::get_gyro(void){
	uint8_t rawdata[6];
	setpage(0);
	readRegister(BNO055_GYRO_DATA_X_LSB_ADDR, rawdata, 6);
    gyro.rawx = (rawdata[1] << 8 | rawdata[0]);
    gyro.rawy = (rawdata[3] << 8 | rawdata[2]);
    gyro.rawz = (rawdata[5] << 8 | rawdata[4]);
    gyro.x = float(gyro.rawx)*rate_scale;
    gyro.y = float(gyro.rawy)*rate_scale;
    gyro.z = float(gyro.rawz)*rate_scale;
}

/// @brief Get the current values from the magnetometer
///        Store the result in mag member, unit is micro Tesla.
/// @param  
void BNO055::get_mag(void){
	uint8_t rawdata[6];

	setpage(0);
	readRegister(BNO055_MAG_DATA_X_LSB_ADDR, rawdata, 6);
    mag.rawx = (rawdata[1] << 8 | rawdata[0]);
    mag.rawy = (rawdata[3] << 8 | rawdata[2]);
    mag.rawz = (rawdata[5] << 8 | rawdata[4]);
    mag.x = float(mag.rawx);
    mag.y = float(mag.rawy);
    mag.z = float(mag.rawz);
}

/// @brief Get the corrected linear acceleration
///        Store the result in lia member.
/// @param  
void BNO055::get_lia(void){
	uint8_t rawdata[6];
	setpage(0);
	readRegister(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, rawdata, 6);
    lia.rawx = (rawdata[1] << 8 | rawdata[0]);
    lia.rawy = (rawdata[3] << 8 | rawdata[2]);
    lia.rawz = (rawdata[5] << 8 | rawdata[4]);
    lia.x = float(lia.rawx)*accel_scale;
    lia.y = float(lia.rawy)*accel_scale;
    lia.z = float(lia.rawz)*accel_scale;
}


/// @brief Get the current gravity vector
///        Store the result in gravity member.
/// @param  
void BNO055::get_grv(void){
	uint8_t rawdata[6];
	setpage(0);
	readRegister(BNO055_GRAVITY_DATA_X_LSB_ADDR, rawdata, 6);
    gravity.rawx = (rawdata[1] << 8 | rawdata[0]);
    gravity.rawy = (rawdata[3] << 8 | rawdata[2]);
    gravity.rawz = (rawdata[5] << 8 | rawdata[4]);
    gravity.x = float(gravity.rawx)*accel_scale;
    gravity.y = float(gravity.rawy)*accel_scale;
    gravity.z = float(gravity.rawz)*accel_scale;
}

/// @brief Get the output quaternion
///        Store the result in quat member.
/// @param  
void BNO055::get_quat(void){
	uint8_t rawdata[8];
	setpage(0);
	readRegister(BNO055_QUATERNION_DATA_W_LSB_ADDR, rawdata, 8);
    quat.raww = (rawdata[1] << 8 | rawdata[0]);
    quat.rawx = (rawdata[3] << 8 | rawdata[2]);
    quat.rawy = (rawdata[5] << 8 | rawdata[4]);
    quat.rawz = (rawdata[7] << 8 | rawdata[6]);
    quat.w = float(quat.raww)/16384.0f;
    quat.x = float(quat.rawx)/16384.0f;
    quat.y = float(quat.rawy)/16384.0f;
    quat.z = float(quat.rawz)/16384.0f;
}

/// @brief Get the current Euler angles 
///        Store the result in euler member.
/// @param  
void BNO055::get_angles(void){
	uint8_t rawdata[6];
	setpage(0);
	readRegister(BNO055_EULER_H_LSB_ADDR, rawdata, 6);
    euler.rawyaw = (rawdata[1] << 8 | rawdata[0]);
    euler.rawroll = (rawdata[3] << 8 | rawdata[2]);
    euler.rawpitch = (rawdata[5] << 8 | rawdata[4]);
    euler.yaw = float(euler.rawyaw)*angle_scale;
    euler.roll = float(euler.rawroll)*angle_scale;
    euler.pitch = float(euler.rawpitch)*angle_scale;
}

/// @brief Get the current temperature
///        Store the result in temperature member.
/// @param  
void BNO055::get_temp(void){
	setpage(0);
	uint8_t rx = readRegister(BNO055_TEMP_ADDR);
    temperature = rx / temp_scale;
}

/// @brief Read the calibration status register
///        Store the result in calib member.
/// @param  
void BNO055::get_calib(void){
	 setpage(0);
	 uint8_t rx = readRegister(BNO055_CALIB_STAT_ADDR);
     calib = rx;
}

/// @brief Read the offset and radius values
/// @param  Store the result in calibration array member.
void BNO055::read_calibration_data(void){
	uint8_t rawdata[22];
    char tempmode = op_mode;
	setpage(0);
    setmode(OPERATION_MODE_CONFIG);
	delay(20000);
	readRegister(ACCEL_OFFSET_X_LSB_ADDR, (uint8_t*)calibration, 22);
    setmode(tempmode);
	delay(10000);
}

/// @brief Write the contents of the calibration array into the registers 
/// @param  
void BNO055::write_calibration_data(void){
	uint8_t rawdata[22];
    char tempmode = op_mode;
	setpage(0);
    setmode(OPERATION_MODE_CONFIG);
	delay(20000);

	writeRegister(ACCEL_OFFSET_X_LSB_ADDR, (uint8_t*)calibration, 22);
    setmode(tempmode);
	delay(10000);
}

