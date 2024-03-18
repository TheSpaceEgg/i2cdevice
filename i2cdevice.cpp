#include "i2cdevice.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <cstdlib>

I2CDevice::I2CDevice(uint8_t bus, uint8_t device)
    : deviceAddress(device) { 
    std::string deviceFile = "/dev/i2c-" + std::to_string(bus);
    fd = open(deviceFile.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "Cannot open the I2C device file." << std::endl;
        throw std::runtime_error("Cannot open the I2C device file.");
    }
}

I2CDevice::~I2CDevice() {
    if (fd >= 0) {
        close(fd);
    }
}

bool I2CDevice::writeRegister(uint8_t reg, uint8_t value, bool err) {
    if (ioctl(fd, I2C_SLAVE, deviceAddress) < 0) {
        if (err) {
            std::cerr << "Failed to set the I2C device address." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        return false;
    }

    uint8_t buffer[2] = {reg, value};
    if (write(fd, buffer, 2) != 2) {
        if (err) {
            std::cerr << "Failed to write to the I2C device." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        return false;
    }
    return true;
}

bool I2CDevice::writeRegister(uint8_t reg, const uint8_t *data, uint8_t length, bool err) {
    if (length == 0 || length > 32) { 
          if (err) {
		  	std::cerr << "Invalid data length for writeRegister." << std::endl;
            std::exit(EXIT_FAILURE); 
        }
        return false;
    }

    if (ioctl(fd, I2C_SLAVE, deviceAddress) < 0) {
    	if (err) {
		  	std::cerr << "Failed to set the I2C device address." << std::endl;
            std::exit(EXIT_FAILURE); 
        }
        return false;
    }

    uint8_t *buffer = new uint8_t[length + 1];
    buffer[0] = reg; 
    memcpy(buffer + 1, data, length); 

    if (write(fd, buffer, length + 1) != length + 1) {
    	if (err) {
        	std::cerr << "Failed to write to the I2C device." << std::endl;
        	std::exit(EXIT_FAILURE); 
    	}
        delete[] buffer;
        return false;
    }

    delete[] buffer;
    return true;
}

bool I2CDevice::readRegister(uint8_t reg, uint32_t &value, uint8_t bytes) {
    if (bytes < 1 || bytes > 32) {
    	if (err) {
        	std::cerr << "Invalid byte count for readRegister." << std::endl;
        	std::exit(EXIT_FAILURE); 
    	}
        return false;
    }

    if (ioctl(fd, I2C_SLAVE, deviceAddress) < 0) {
    	if (err) {
        	std::cerr << "Failed to set the I2C device address." << std::endl;
        	std::exit(EXIT_FAILURE); 
    	}
        return false;
    }

    if (write(fd, &reg, 1) != 1) {
    	if (err) {
        	std::cerr << "Failed to write the register address to the I2C device." << std::endl;
        	std::exit(EXIT_FAILURE); 
    	}
        return false;
    }

    uint8_t buffer[4] = {0};
    if (read(fd, buffer, bytes) != bytes) {
    	if (err) {
        	std::cerr << "Failed to read from the I2C device." << std::endl;
        	std::exit(EXIT_FAILURE); 
    	}
        return false;
    }

    value = 0;
    for (int i = 0; i < bytes; i++) {
        value |= (static_cast<uint32_t>(buffer[i]) << (8 * (bytes - i - 1)));
    }

    return true;
}

