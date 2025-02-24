#include "bsp_wire.h"

TwoWire Wire; // 创建全局Wire对象

void TwoWire::begin(void) {
    // LL_init();已经初始化
}

void TwoWire::beginTransmission(uint8_t address) {
    _address = address;
    _writeIndex = 0;
}

uint8_t TwoWire::endTransmission(bool stop) {
    // 等待I2C就绪
    while(LL_I2C_IsActiveFlag_BUSY(I2C1));
    
    // 发送起始信号
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1));
    
    // 发送地址(写模式)
    LL_I2C_TransmitData8(I2C1, (_address << 1) | 0);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C1));
    LL_I2C_ClearFlag_ADDR(I2C1);
    
    // 发送数据
    for(uint8_t i = 0; i < _writeIndex; i++) {
        LL_I2C_TransmitData8(I2C1, _buffer[i]);
        while(!LL_I2C_IsActiveFlag_TXE(I2C1));
    }
    
    // 根据参数决定是否发送停止信号
    if (stop) {
        LL_I2C_GenerateStopCondition(I2C1);
    }
    
    return 0; // 成功返回0
}

int TwoWire::write(uint8_t data) {
    if(_writeIndex < BUFFER_LENGTH) {
        _buffer[_writeIndex++] = data;
        return 1;
    }
    return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) {
    if(quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }
    
    // 等待I2C就绪
    while(LL_I2C_IsActiveFlag_BUSY(I2C1));
    
    // 发送起始信号
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1));
    
    // 发送地址(读模式)
    LL_I2C_TransmitData8(I2C1, (address << 1) | 1);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C1));
    LL_I2C_ClearFlag_ADDR(I2C1);
    
    // 接收数据
    _readIndex = 0;
    while(quantity--) {
        if(quantity == 0) {
            LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK); // 最后一个字节发送NACK
        }
        while(!LL_I2C_IsActiveFlag_RXNE(I2C1));
        _buffer[_readIndex++] = LL_I2C_ReceiveData8(I2C1);
    }
    
    // 发送停止信号
    LL_I2C_GenerateStopCondition(I2C1);
    
    return _readIndex;
}

int TwoWire::available(void) {
    return _readIndex;
}

int TwoWire::read(void) {
    if(_readIndex > 0) {
        return _buffer[--_readIndex];
    }
    return -1;
}
