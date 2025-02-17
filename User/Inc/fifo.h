#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fifo {
  uint8_t *buffer;   // 缓冲区
  uint32_t capacity; // 大小
  uint32_t in;       // 入口位置
  uint32_t out;      // 出口位置
};

// 取a和b中最小值
#define fifo_min(a, b) (((a) < (b)) ? (a) : (b))

void fifo_init(struct fifo *fifo, uint8_t *buffer, uint32_t capacity);
// 获取FIFO中可写入的空闲空间
static inline uint32_t fifo_write_available(struct fifo *fifo)
{
    return fifo->capacity - (fifo->in - fifo->out);
}

// 获取FIFO中从当前写入位置到缓冲区末尾的连续可用空间
static inline uint32_t fifo_write_available_to_end(struct fifo *fifo)
{
    uint32_t write_pos = fifo->in & (fifo->capacity - 1);  // 当前写入位置
    return fifo->capacity - write_pos;  // 从写入位置到缓冲区末尾的空间
}

// 获取FIFO中可读取的数据长度
static inline uint32_t fifo_read_available(struct fifo *fifo)
{
    return fifo->in - fifo->out;
}

// 获取FIFO中从当前读取位置到缓冲区末尾的连续可用空间
static inline uint32_t fifo_read_available_to_end(struct fifo *fifo)
{
    uint32_t read_pos = fifo->out & (fifo->capacity - 1);  // 当前读取位置
    return fifo->capacity - read_pos;  // 从读取位置到缓冲区末尾的空间
}

// 写入数据到FIFO
uint32_t fifo_write(struct fifo *fifo, const uint8_t *buffer, uint32_t len);
// 读取数据从FIFO
uint32_t fifo_read(struct fifo *fifo, uint8_t *buffer, uint32_t len);
// 查看数据从FIFO
uint32_t fifo_peek(struct fifo *fifo, uint8_t *buffer, uint32_t len);
uint32_t fifo_commit_read(struct fifo *fifo, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif // _FIFO_H_
