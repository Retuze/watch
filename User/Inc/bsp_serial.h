#ifndef __BSP_SERIAL_H
#define __BSP_SERIAL_H

#include "main.h"
#include "fifo.h"
#ifdef __cplusplus
extern "C"
{
#endif

void serial_init(int baudrate);
void serial_send(uint8_t *data, uint16_t length);
void serial_recv(struct fifo *fifo);
void serial_process(void);

#ifdef __cplusplus
}
#endif

#endif
