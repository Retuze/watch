#include "fifo.h"
#include <string.h>

// 初始化FIFO
void fifo_init(struct fifo *fifo, uint8_t *buffer, uint32_t capacity)
{
    // 确保容量是2的幂
    if ((capacity & (capacity - 1)) != 0) {
        // printf("kfifo_init: capacity is not a power of 2\n");
        return;
    }
    fifo->buffer = buffer;
    fifo->capacity = capacity;
    fifo->in = 0;
    fifo->out = 0;
}   

// 写入数据到FIFO
uint32_t fifo_write(struct fifo *fifo, const uint8_t *buffer, uint32_t len)
{
    // 确保len不超过fifo的剩余空间
    len = fifo_min(len, fifo_write_available(fifo));
    
    // 计算写入位置和可写入长度
    uint32_t write_pos = fifo->in & (fifo->capacity - 1);
    uint32_t to_end = fifo_min(len, fifo->capacity - write_pos);
    
    // 写入第一段数据
    memcpy(fifo->buffer + write_pos, buffer, to_end);
    // 如果有需要，写入第二段数据（环形缓冲区回环）
    if (len > to_end) {
        memcpy(fifo->buffer, buffer + to_end, len - to_end);
    }
    
    fifo->in += len;
    return len;
}

// 从FIFO读取数据
uint32_t fifo_read(struct fifo *fifo, uint8_t *buffer, uint32_t len)
{
    // 确保len不超过fifo中的数据量
    len = fifo_min(len, fifo_read_available(fifo));
    
    // 计算读取位置和可读取长度
    uint32_t read_pos = fifo->out & (fifo->capacity - 1);
    uint32_t to_end = fifo_min(len, fifo->capacity - read_pos);
    
    // 读取第一段数据
    memcpy(buffer, fifo->buffer + read_pos, to_end);
    // 如果有需要，读取第二段数据（环形缓冲区回环）
    if (len > to_end) {
        memcpy(buffer + to_end, fifo->buffer, len - to_end);
    }
    
    fifo->out += len;
    return len;
}

// 查看FIFO中的数据但不移动读指针
uint32_t fifo_peek(struct fifo *fifo, uint8_t *buffer, uint32_t len)
{
    // 确保len不超过fifo中的数据量
    len = fifo_min(len, fifo->in - fifo->out);
    
    // 计算读取位置和可读取长度
    uint32_t read_pos = fifo->out & (fifo->capacity - 1);
    uint32_t to_end = fifo_min(len, fifo->capacity - read_pos);
    
    // 读取第一段数据
    memcpy(buffer, fifo->buffer + read_pos, to_end);
    // 如果有需要，读取第二段数据（环形缓冲区回环）
    if (len > to_end) {
        memcpy(buffer + to_end, fifo->buffer, len - to_end);
    }
    
    return len;
}

// 提交读取操作，移动读指针
uint32_t fifo_commit_read(struct fifo *fifo, uint32_t len)
{
    len = fifo_min(len, fifo->in - fifo->out);
    fifo->out += len;
    return len;
}



