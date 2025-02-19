#include <rthw.h>
#include <rtthread.h>
#include "bsp_gpio.h"

int rtthread_startup(void);

static struct rt_thread test_thread;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t test_thread_stack[256];

void test_thread_entry(void *parameter)
{
    (void)parameter;
    while (1) {
        pin_write(PC13, 1);
        rt_thread_mdelay(1000);
        pin_write(PC13, 0);
        rt_thread_mdelay(1000);
    }
}

int rtthread_startup(void)
{
    rt_hw_interrupt_disable();
    extern void *rt_heap_begin_get(void);
    extern void *rt_heap_end_get(void);
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();

#ifdef RT_USING_SIGNALS
    /* signal system initialization */
    rt_system_signal_init();
#endif /* RT_USING_SIGNALS */

    /* create init_thread */
    rt_thread_init(&test_thread, "test", test_thread_entry, 0, test_thread_stack,256 , 2, 10);
    rt_thread_startup(&test_thread);

    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

    /* start scheduler */
    // rt_system_scheduler_start();

    /* never reach here */
    return 0;
}
