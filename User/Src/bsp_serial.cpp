#include "bsp_serial.h"
#include <cstring>
#include <cstdio>

HardwareSerial Serial;

void HardwareSerial::begin(unsigned long baudrate) {
    // 初始化串口
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    LL_USART_SetBaudRate(USART1, RCC_Clocks.PCLK2_Frequency, baudrate);

    // 初始化发送和接收FIFO
    fifo_init(&tx_fifo, tx_buffer, TX_BUFFER_SIZE);
    fifo_init(&rx_fifo, rx_buffer, RX_BUFFER_SIZE);

    // 启用 DMA 模式
    /* 配置地址 */
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, LL_USART_DMA_GetRegAddr(USART1), (uint32_t)rx_buffer,
                           LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));

    /* 设置缓冲区大小 */
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, RX_BUFFER_SIZE);
    // 启用 IDLE（空闲中断）
    LL_USART_EnableIT_IDLE(USART1);
    // 启用DMA半满中断
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);
    // 启用DMA全满中断
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
    // 启用DMA传输错误中断
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);

    // /* 启动串口接收DMA通道  并启用串口接收DMA */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_USART_EnableDMAReq_RX(USART1);

    // 配置发送DMA
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)tx_buffer, LL_USART_DMA_GetRegAddr(USART1),
                           LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));

    // 启用发送DMA半传输完成中断
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_4);
    // 启用发送DMA传输完成中断
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
    // 启用发送DMA错误中断
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);

    // 启用串口发送DMA请求
    LL_USART_EnableDMAReq_TX(USART1);
}

uint8_t HardwareSerial::write(const uint8_t *buffer, uint8_t size) {
    uint16_t bytes_written = 0;
    uint32_t timeout;

    while(bytes_written < size) {
        // 使用可配置的超时值
        timeout = SERIAL_WRITE_TIMEOUT;

        // 等待FIFO有空间可写
        while(fifo_write_available(&tx_fifo) == 0) {
            if(--timeout == 0) {
                return bytes_written;
            }
            // 可以考虑在这里添加yield或短暂休眠
            // yield(); // 如果系统支持
        }

        uint16_t available = fifo_write_available(&tx_fifo);
        uint16_t to_write = fifo_min(available, size - bytes_written);
        fifo_write(&tx_fifo, buffer + bytes_written, to_write);
        bytes_written += to_write;

        // 如果DMA没有在发送，启动新的传输
        if(!LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_4)) {
            _start_next_dma_transfer();
        }
    }

    return bytes_written;
}

void HardwareSerial::_start_next_dma_transfer(void) {
    uint16_t send_len = fifo_read_available(&tx_fifo);
    if(send_len > 0) {
        uint16_t read_pos = tx_fifo.out & (TX_BUFFER_SIZE - 1);
        uint16_t to_end = fifo_min(send_len, TX_BUFFER_SIZE - read_pos);
        send_len = fifo_min(send_len, to_end);  // 简化判断逻辑

        dma_tx_size = send_len;
        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&tx_buffer[read_pos]);
        LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, send_len);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    } else {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
        dma_tx_size = 0;
    }
}

void HardwareSerial::_handle_dma_rx(void) {
    uint32_t current_dma_pos = HardwareSerial::RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    uint32_t last_fifo_pos = HardwareSerial::rx_fifo.in & (HardwareSerial::RX_BUFFER_SIZE - 1);

    uint16_t recv_size;
    if(current_dma_pos >= last_fifo_pos) {
        recv_size = current_dma_pos - last_fifo_pos;
    } else {
        recv_size = HardwareSerial::RX_BUFFER_SIZE - last_fifo_pos + current_dma_pos;
    }

    rx_fifo.in += recv_size;
}

uint8_t HardwareSerial::print(const char* str) {
    if (!str) return 0;
    return write((const uint8_t*)str, strlen(str));
}

uint8_t HardwareSerial::print(int num) {
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d", num);
    if (len > 0) {
        return write((const uint8_t*)buffer, len);
    }
    return 0;
}

uint8_t HardwareSerial::print(float num, int digits) {
    char buffer[16];
    char format[8];
    snprintf(format, sizeof(format), "%%.%df", digits);
    int len = snprintf(buffer, sizeof(buffer), format, num);
    if (len > 0) {
        return write((const uint8_t*)buffer, len);
    }
    return 0;
}

uint8_t HardwareSerial::println(const char* str) {
    uint8_t n = print(str);
    uint8_t r = write((const uint8_t*)"\r\n", 2);
    return n + r;
}

uint8_t HardwareSerial::println(int num) {
    uint8_t n = print(num);
    uint8_t r = write((const uint8_t*)"\r\n", 2);
    return n + r;
}

uint8_t HardwareSerial::println(float num, int digits) {
    uint8_t n = print(num, digits);
    uint8_t r = write((const uint8_t*)"\r\n", 2);
    return n + r;
}

uint8_t HardwareSerial::println(void) {
    return write((const uint8_t*)"\r\n", 2);
}

uint8_t HardwareSerial::print(long num) {
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%ld", num);
    if (len > 0) {
        return write((const uint8_t*)buffer, len);
    }
    return 0;
}

uint8_t HardwareSerial::println(long num) {
    uint8_t n = print(num);
    uint8_t r = write((const uint8_t*)"\r\n", 2);
    return n + r;
}


void USART1_IRQHandler(void) {
    // 处理 IDLE 中断
    if(LL_USART_IsActiveFlag_IDLE(USART1)) {
        LL_USART_ClearFlag_IDLE(USART1);
        Serial._handle_dma_rx();
    }
}


void DMA1_Channel5_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_HT5(DMA1)) {
        LL_DMA_ClearFlag_HT5(DMA1);
        Serial._handle_dma_rx();
    } else if(LL_DMA_IsActiveFlag_TC5(DMA1)) {
        LL_DMA_ClearFlag_TC5(DMA1);
        Serial._handle_dma_rx();
    } else if(LL_DMA_IsActiveFlag_TE5(DMA1)) {
        LL_DMA_ClearFlag_GI5(DMA1);
        // 传输错误处理
        fifo_init(&Serial.rx_fifo, Serial.rx_buffer, HardwareSerial::RX_BUFFER_SIZE);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    }
}

void DMA1_Channel4_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_HT4(DMA1)) {
        LL_DMA_ClearFlag_HT4(DMA1);
        uint16_t remain_len = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_4);
        Serial.tx_fifo.out += Serial.dma_tx_size - remain_len;
        Serial.dma_tx_size = remain_len;
    } else if(LL_DMA_IsActiveFlag_TC4(DMA1)) {
        LL_DMA_ClearFlag_TC4(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
        Serial.tx_fifo.out += Serial.dma_tx_size;
        Serial._start_next_dma_transfer();
    } else if(LL_DMA_IsActiveFlag_TE4(DMA1)) {
        LL_DMA_ClearFlag_GI4(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    }
}