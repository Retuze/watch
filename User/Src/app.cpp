#include "bsp_gpio.h"
#include "bsp_serial.h"
#include "rtthread.h"
#include "stm32f1xx_ll_utils.h"
#include "app.h"
#include "thread_native.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern int rtthread_startup(void);


#ifdef __cplusplus
}
#endif



void app_lunch(void)
{
    pin_mode(PC13, OUTPUT_PUSH_PULL);
    serial_init(9600);
    
    rtthread_startup();



    // auto test_thread = std::thread([](void *parameter) {
    //     (void)parameter;
    //     while (1) {
    //         pin_write(PC13, 1);
    //         rt_thread_mdelay(1000);
    //         pin_write(PC13, 0);
    //         rt_thread_mdelay(1000);
    //     }
    // },nullptr);
    // test_thread.join();
    rt_system_scheduler_start();
    while (1)
    {
        serial_process();
        // pin_write(PC13, 1);
        // LL_mDelay(500);
        // pin_write(PC13, 0);
        // LL_mDelay(500);
    }
}

