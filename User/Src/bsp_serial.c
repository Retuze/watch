#include "bsp_serial.h"

#include "fifo.h"

// 发送缓冲区大小（环形缓冲区）
#define TX_BUFFER_SIZE  64
// 接收缓冲区大小（环形缓冲区）
#define RX_BUFFER_SIZE  64

/*
 * 定义全局结构体，用于组织串口发送和接收相关数据及状态
 */
struct SerialPort {
    uint8_t tx_buffer[TX_BUFFER_SIZE];
    uint8_t rx_buffer[RX_BUFFER_SIZE];
    struct fifo tx_fifo;
    struct fifo rx_fifo;
};

// 定义全局串口状态变量
static struct SerialPort g_serial;

/**
 * @brief 初始化串口，同时初始化全局结构体中的各个索引和标记
 * @param baudrate 波特率
 */
void serial_init(int baudrate) {
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    LL_USART_SetBaudRate(USART1, RCC_Clocks.PCLK2_Frequency, baudrate);

    // 初始化发送和接收FIFO
    fifo_init(&g_serial.tx_fifo, g_serial.tx_buffer, TX_BUFFER_SIZE);
    fifo_init(&g_serial.rx_fifo, g_serial.rx_buffer, RX_BUFFER_SIZE);

    // 启用 DMA 模式
    /* 配置地址 */
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, LL_USART_DMA_GetRegAddr(USART1), (uint32_t)g_serial.rx_buffer,
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
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)g_serial.tx_buffer, LL_USART_DMA_GetRegAddr(USART1),
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

static uint16_t serial_send_len = 0;

void serial_send(uint8_t *data, uint16_t length) {
    uint16_t bytes_written = 0;
    uint32_t timeout;
    
    while (bytes_written < length) {
        timeout = 100000000;  // 设置适当的超时值
        while (fifo_write_available(&g_serial.tx_fifo) == 0) {
            if (--timeout == 0) {
                // 超时处理，可以返回错误码
                return;
            }
            __NOP();
        }
        
        uint16_t available = fifo_write_available(&g_serial.tx_fifo);
        // 计算这次可以写入的数据量
        uint16_t to_write = fifo_min(available, length - bytes_written);
        // 写入数据
        fifo_write(&g_serial.tx_fifo, data + bytes_written, to_write);
        bytes_written += to_write;
        
        // 如果DMA没有在发送，立即启动发送
        if(!LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_4)) {
            uint16_t send_len = fifo_read_available(&g_serial.tx_fifo);
            // 计算读取位置
            uint16_t read_pos = g_serial.tx_fifo.out & (TX_BUFFER_SIZE - 1);
            // 计算到缓冲区末尾的剩余空间
            uint16_t to_end = fifo_min(send_len, TX_BUFFER_SIZE - read_pos);
            // 如果数据跨越了缓冲区末尾，先只发送到末尾的部分
            if(send_len > to_end) {
                send_len = to_end;
            }
            serial_send_len = send_len;
            // 配置DMA传输
            LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&g_serial.tx_buffer[read_pos]);
            LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, send_len);
            LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
        }
    }
    return;
}

/**
 * @brief 数据处理函数：对接收到的数据进行应用层处理。
 * @param data 数据指针
 * @param length 数据长度
 * @param out 输出指针
 * 注意：此函数在主循环中调用（中断外），加工/解析数据后可交由上层处理。
 */
void serial_recv(struct fifo *fifo) {
    (void)fifo;
    // 给应用层处理
    uint8_t data[200];
    uint16_t len = fifo_read_available(fifo);
    fifo_read(fifo, data, len);
    serial_send(data, len);
}

void serial_process(void) {
    // 如果没有数据可读，则直接返回
    if(fifo_read_available(&g_serial.rx_fifo) == 0) {
        return;
    }
    serial_recv(&g_serial.rx_fifo);
}

static void handle_dma_rx(void) {
    uint32_t current_dma_pos = RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    uint32_t last_fifo_pos = g_serial.rx_fifo.in & (RX_BUFFER_SIZE - 1);

    uint16_t recv_size;
    if(current_dma_pos >= last_fifo_pos) {
        recv_size = current_dma_pos - last_fifo_pos;
    } else {
        recv_size = RX_BUFFER_SIZE - last_fifo_pos + current_dma_pos;
    }

    g_serial.rx_fifo.in += recv_size;
}

/**
 * @brief USART1 中断服务函数，负责处理数据接收和发送中断
 */
void USART1_IRQHandler(void) {
    // 处理 IDLE 中断
    if(LL_USART_IsActiveFlag_IDLE(USART1)) {
        LL_USART_ClearFlag_IDLE(USART1);
        handle_dma_rx();
    }
}

/**
 * @brief This function handles DMA1 channel5 global interrupt.
 */
void DMA1_Channel5_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_HT5(DMA1)) {
        LL_DMA_ClearFlag_HT5(DMA1);
        handle_dma_rx();
    } else if(LL_DMA_IsActiveFlag_TC5(DMA1)) {
        LL_DMA_ClearFlag_TC5(DMA1);
        handle_dma_rx();
    } else if(LL_DMA_IsActiveFlag_TE5(DMA1)) {
        LL_DMA_ClearFlag_GI5(DMA1);
        // 传输错误处理
        fifo_init(&g_serial.rx_fifo, g_serial.rx_buffer, RX_BUFFER_SIZE);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    }
}

/**
 * @brief This function handles DMA1 channel4 global interrupt.
 */
void DMA1_Channel4_IRQHandler(void) {
    if(LL_DMA_IsActiveFlag_HT4(DMA1)) {
        LL_DMA_ClearFlag_HT4(DMA1);
        // 处理半传输完成中断
        uint16_t remain_len = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_4);
        g_serial.tx_fifo.out += serial_send_len - remain_len;
        serial_send_len = remain_len;
    } else if(LL_DMA_IsActiveFlag_TC4(DMA1)) {
        LL_DMA_ClearFlag_TC4(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
        // 处理传输完成中断
        g_serial.tx_fifo.out += serial_send_len;
        uint16_t send_len = fifo_read_available(&g_serial.tx_fifo);
        // 检查是否还有剩余数据需要发送
        if(send_len > 0) {
            // 计算读取位置
            uint16_t read_pos = g_serial.tx_fifo.out & (TX_BUFFER_SIZE - 1);
            // 计算到缓冲区末尾的剩余空间
            uint16_t to_end = fifo_min(send_len, TX_BUFFER_SIZE - read_pos);
            // 如果数据跨越了缓冲区末尾，先只发送到末尾的部分
            if(send_len > to_end) {
                send_len = to_end;
            }
            serial_send_len = send_len;
            // 配置DMA传输
            LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&g_serial.tx_buffer[read_pos]);
            LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, send_len);
            LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
        } else {
            LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
            serial_send_len = 0;
        }
    } else if(LL_DMA_IsActiveFlag_TE4(DMA1)) {
        LL_DMA_ClearFlag_GI4(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    }
}
