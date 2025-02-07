#include "bsp_gpio.h"
#include "main.h"

static GPIO_TypeDef *GetGPIOx(uint16_t pin)
{
    switch (pin / 16)
    {
    case 0:
        return GPIOA;
    case 1:
        return GPIOB;
    case 2:
        return GPIOC;
    case 3:
        return GPIOD;
    case 4:
        return GPIOE;
    default:
        return 0;
    }
}

static uint32_t GetGPIOPin(uint16_t pin)
{
    switch (pin % 16)
    {
    case 0:
        return LL_GPIO_PIN_0;
    case 1:
        return LL_GPIO_PIN_1;
    case 2:
        return LL_GPIO_PIN_2;
    case 3:
        return LL_GPIO_PIN_3;
    case 4:
        return LL_GPIO_PIN_4;
    case 5:
        return LL_GPIO_PIN_5;
    case 6:
        return LL_GPIO_PIN_6;
    case 7:
        return LL_GPIO_PIN_7;
    case 8:
        return LL_GPIO_PIN_8;
    case 9:
        return LL_GPIO_PIN_9;
    case 10:
        return LL_GPIO_PIN_10;
    case 11:
        return LL_GPIO_PIN_11;
    case 12:
        return LL_GPIO_PIN_12;
    case 13:
        return LL_GPIO_PIN_13;
    case 14:
        return LL_GPIO_PIN_14;
    case 15:
        return LL_GPIO_PIN_15;
    default:
        return 0;
    }
}

void pin_mode(uint16_t pin, uint8_t mode)
{
    LL_GPIO_SetPinMode(GetGPIOx(pin), GetGPIOPin(pin), mode);
}

void pin_write(uint16_t pin, uint8_t value)
{
    value ? LL_GPIO_SetOutputPin(GetGPIOx(pin), GetGPIOPin(pin)) : LL_GPIO_ResetOutputPin(GetGPIOx(pin), GetGPIOPin(pin));
}

void pin_toggle(uint16_t pin)
{
    LL_GPIO_TogglePin(GetGPIOx(pin), GetGPIOPin(pin));
}

uint8_t pin_read(uint16_t pin)
{
    return LL_GPIO_IsInputPinSet(GetGPIOx(pin), GetGPIOPin(pin));
}
