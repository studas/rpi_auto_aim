#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP

#include <stdio.h>

class I2CSlave {
private:
    uint16_t sda_io;
    uint16_t scl_io;
    uint16_t addr;
    i2c_port_t port;

public:
    I2CSlave(uint16_t sda_io, uint16_t scl_io, uint16_t addr, int port);
    ~I2CSlave();
    char* read_buffer();
    void write_buffer(char* data);
};

#endif