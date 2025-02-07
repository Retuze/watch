#include "bsp_gpio.h"
#include "main.h"
#include "app.h"

void app_lunch(void)
{
    while (1)
    {
        pin_write(PC13, 1);
        LL_mDelay(500);
        pin_write(PC13, 0);
        LL_mDelay(500);
    }
}

