#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"

#include "gpio_t.hpp"

static const char *TAG = "gpio_t";

gpio_t::gpio_t(int gpio_pin) {
    this->gpio_pin = (gpio_num_t)gpio_pin;
    this->gpio_mode = GPIO_MODE_OUTPUT;
    this->pull_mode = GPIO_FLOATING;
    gpio_reset_pin(this->gpio_pin);
    gpio_set_direction(this->gpio_pin, this->gpio_mode);
}

gpio_t::gpio_t(int gpio_pin, gpio_mode_t gpio_mode, gpio_pull_mode_t pull_mode) 
    : gpio_mode(gpio_mode),pull_mode(pull_mode) {
    this->gpio_pin = (gpio_num_t)gpio_pin;
    gpio_reset_pin(this->gpio_pin);
    gpio_set_direction(this->gpio_pin, this->gpio_mode);
    gpio_set_pull_mode(this->gpio_pin, this->pull_mode);
}

gpio_t::~gpio_t() {
    gpio_reset_pin(this->gpio_pin);
}

void gpio_t::set_level(gpio_level_t level) {
    if(gpio_mode == GPIO_MODE_INPUT && gpio_mode == GPIO_MODE_DISABLE){
        ESP_LOGE(TAG, "Cannot set level on input pin");
        return;
    }
    gpio_set_level(this->gpio_pin, level);
}

gpio_level_t gpio_t::get_level() {
    int level = gpio_get_level(this->gpio_pin);
    return static_cast<gpio_level_t>(level);
}
