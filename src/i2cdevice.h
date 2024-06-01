#ifndef I2CDEVICE_H
#define I2CDEVICE_H


#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>

class I2CDevice {
public:
    I2CDevice(uint8_t bus, uint8_t device);
    ~I2CDevice();
    
    bool writeRegister(uint8_t reg, uint8_t value, bool err = false);
	  bool writeRegister(uint8_t reg, const uint8_t* data, uint8_t length, bool err = false);

    bool readRegister(uint8_t reg, uint8_t* value, uint8_t bytes, bool err = false);

private:
    int fd; 
    uint8_t deviceAddress; 
    void openDevice();
};

#endif 

