#ifndef __ARDUINO_H
#define __ARDUINO_H

#pragma once


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "rtthread.h"

typedef unsigned char byte;
typedef unsigned int word;
typedef bool boolean;

static inline uint32_t millis(void){return rt_tick_get();}
static inline uint32_t micros(void){return rt_tick_get()*1000;}
static inline void delay(uint32_t ms){rt_thread_mdelay(ms);}
static inline void delayMicroseconds(uint32_t us){rt_thread_mdelay(us/1000);}


#endif
