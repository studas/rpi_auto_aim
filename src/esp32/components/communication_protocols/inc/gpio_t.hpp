#ifndef __GPIO_HPP__
#define __GPIO_HPP__

#include "driver/gpio.h"

enum gpio_level_t{
    LOW = 0,
    HIGH = 1,
};

class gpio_t{
    private:
        gpio_num_t gpio_pin;
        gpio_mode_t gpio_mode;
        gpio_pull_mode_t pull_mode;

    public:
        gpio_t(int gpio_pin, gpio_mode_t gpio_mode, gpio_pull_mode_t pull_mode);
        gpio_t(int gpio_pin);
        ~gpio_t();
        void set_level(gpio_level_t level);
        gpio_level_t get_level();
};

#endif // __GPIO_HPP__
