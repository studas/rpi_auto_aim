#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

class Servo {
private:
    int gpio_pin;
    ledc_channel_t channel;
    uint32_t pulse_width, min_pulse_width, max_pulse_width;

public:
    Servo(int gpio_pin, ledc_channel_t channel);
    ~Servo();
    void add_pulse_width(uint32_t change);
    void set_angle(int angle);
    void set_min_pulse_width(uint32_t min_pulse_width);
    void set_max_pulse_width(uint32_t max_pulse_width);
    void update();
};

#endif // SERVO_CONTROLLER_HPP
