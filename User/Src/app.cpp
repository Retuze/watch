#include "app.h"

#include <thread>
#include <chrono>

#include "bsp_gpio.h"
#include "bsp_serial.h"
#include "rtthread.h"
#include "stm32f1xx_ll_utils.h"
#include <sys/time.h>


using namespace std::chrono_literals;

void app_lunch(void) {
    pin_mode(PC13, OUTPUT_PUSH_PULL);
    serial_init(9600);
    std::thread t([]() {
        for(int i = 0; i < 10; i++) {
            pin_write(PC13, 1);
            std::this_thread::sleep_for(100ms);
            pin_write(PC13, 0);
            std::this_thread::sleep_for(100ms);
        }
    });
    t.join();
    while(1) {
            pin_write(PC13, 1);
            std::this_thread::sleep_for(500ms);
            pin_write(PC13, 0);
            std::this_thread::sleep_for(500ms);
        // serial_process();
    }
}
