#ifndef __BSP_SERIAL_H
#define __BSP_SERIAL_H

#include "fifo.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void USART1_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class HardwareSerial {
   public:

    // 基本串口操作
    void begin(unsigned long baudrate);
    void end();

    // 写操作
    uint8_t write(uint8_t byte) { return write(&byte, 1); }
    uint8_t write(const uint8_t *buffer, uint8_t size);

    // 读操作
    int available();
    int read();
    uint8_t readBytes(uint8_t *buffer, uint8_t length);
    uint8_t readBytesUntil(char terminator, uint8_t *buffer, uint8_t length);
    void serialEventRun(void);

    // 打印功能
    uint8_t print(const char *str);
    uint8_t print(int num);
    uint8_t print(float num, int digits = 2);
    uint8_t print(long num);
    uint8_t println(const char *str);
    uint8_t println(int num);
    uint8_t println(float num, int digits = 2);
    uint8_t println(long num);
    uint8_t println(void);

    // 缓冲区操作
    void flush();

   private:
    constexpr static uint8_t RX_BUFFER_SIZE = 64;
    constexpr static uint8_t TX_BUFFER_SIZE = 64;
    constexpr static uint32_t SERIAL_WRITE_TIMEOUT = 10000;
    struct fifo rx_fifo;  // 接收缓冲区
    struct fifo tx_fifo;  // 发送缓冲区
    uint8_t rx_buffer[RX_BUFFER_SIZE];
    uint8_t tx_buffer[TX_BUFFER_SIZE];
    bool _initialized;  // 串口初始化状态
    uint16_t dma_tx_size;
    void _start_next_dma_transfer(void);
    void _handle_dma_rx(void);

    // 中断处理函数声明为友元函数
    friend void USART1_IRQHandler(void);
    friend void DMA1_Channel5_IRQHandler(void);
    friend void DMA1_Channel4_IRQHandler(void);
};

// 创建全局对象
extern HardwareSerial Serial;  // 改为Arduino标准的Serial命名

#endif  // __BSP_SERIAL_H
