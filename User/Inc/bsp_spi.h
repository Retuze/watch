#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include <cstdint>
#include "main.h"

// SPI 数据模式
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

// SPI 位序
#define MSBFIRST 0
#define LSBFIRST 1

class SPIClass {
public:
    // 构造函数
    SPIClass();
    
    // 初始化 SPI
    void begin();
    void end();
    
    // 设置时钟分频
    void setClockDivider(uint8_t div);
    
    // 设置位序
    void setBitOrder(uint8_t bitOrder);
    
    // 设置数据模式
    void setDataMode(uint8_t dataMode);
    
    // 传输函数
    uint8_t transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    void transfer(void *buf, uint16_t count);
    
    // 片选控制
    void beginTransaction();
    void endTransaction();
    
private:
    // 后续添加必要的私有成员
};

// 创建默认 SPI 实例
extern SPIClass SPI;

#endif /* __BSP_SPI_H */
