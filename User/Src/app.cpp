#include "bsp_gpio.h"
#include "bsp_serial.h"
#include "stm32f1xx_ll_utils.h"
#include "app.h"

void app_lunch(void)
{
    pin_mode(PC13, OUTPUT_PUSH_PULL);
    serial_init(9600);
    uint8_t data[] = " Hello, World!\n";
    for(int i = 0; i < 20; i++) {
        serial_send(data, sizeof(data));
        // LL_mDelay(30);
    }
    while (1)
    {
        serial_process();
        // pin_write(PC13, 1);
        // LL_mDelay(500);
        // pin_write(PC13, 0);
        // LL_mDelay(500);
    }
}

