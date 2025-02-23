/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__


#define RT_THREAD_PRIORITY_MAX  32
#define RT_TICK_PER_SECOND  1000
#define RT_ALIGN_SIZE   4
#define RT_NAME_MAX    8


//#define RT_DEBUG

#define RT_DEBUG_INIT 0

//#define RT_USING_OVERFLOW_CHECK


//#define RT_USING_HOOK
//#define RT_USING_IDLE_HOOK

#define RT_USING_TIMER_SOFT         0
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif
#define RT_TIMER_THREAD_PRIO        4
#define RT_TIMER_THREAD_STACK_SIZE  512



#define RT_USING_SEMAPHORE
// #define RT_USING_MUTEX
// #define RT_USING_EVENT
// #define RT_USING_SIGNALS
// #define RT_USING_MAILBOX
//#define RT_USING_MESSAGEQUEUE


//#define RT_USING_MEMPOOL

#define RT_USING_HEAP
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP

//#define RT_USING_TINY_SIZE


#endif
