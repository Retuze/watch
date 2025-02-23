#include "main.h"

#include <thread>
#include <chrono>
#include "bsp_gpio.h"
#include "bsp_serial.h"



using namespace std::chrono_literals;

int main(void)
{
    LL_init();
    std::thread t([]() {
        while(1) {
            pin_write(PC13, 1);
            std::this_thread::sleep_for(500ms);
            pin_write(PC13, 0);
            std::this_thread::sleep_for(500ms);
        }
    });
    t.join();

    while(1) {
        pin_write(PC13, 1);
        std::this_thread::sleep_for(500ms);
        pin_write(PC13, 0);
        std::this_thread::sleep_for(500ms);
    }
    return 0;
}

