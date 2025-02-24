#ifndef __BSP_WIRE_H
#define __BSP_WIRE_H

#include <cstdio>
#include "main.h"

#define BUFFER_LENGTH 32  // I2C缓冲区大小

class TwoWire {
public:
    void begin(void);
    void beginTransmission(uint8_t address);
    uint8_t endTransmission(bool stop = true);
    int write(uint8_t data);
    uint8_t requestFrom(uint8_t address, uint8_t quantity);
    int available(void);
    int read(void);
    void setClock(uint32_t speed){_speed = speed;}

private:
    uint8_t _address;
    uint8_t _buffer[BUFFER_LENGTH];
    uint8_t _writeIndex;
    uint8_t _readIndex;
    uint32_t _speed;
};

extern TwoWire Wire;

#endif