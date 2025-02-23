#include <rthw.h>
#include <rtthread.h>

void main_thread_entry(void *parameter)
{
    (void)parameter;
    extern int main(void);
    main();
}

int entry(void)
{
    rt_hw_interrupt_disable();

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();

#ifdef RT_USING_SIGNALS
    /* signal system initialization */
    rt_system_signal_init();
#endif /* RT_USING_SIGNALS */

    rt_thread_t t = rt_thread_create("init", main_thread_entry, RT_NULL, 1024, 2, 10);
    rt_thread_startup(t);

    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return 0;
}
