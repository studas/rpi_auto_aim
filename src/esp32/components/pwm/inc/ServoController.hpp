#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

class Servo {
private:
    int gpio_pin;
    ledc_channel_t channel;
    int angle;
    uint32_t min_pulse_width, max_pulse_width;

public:
    Servo(int gpio_pin, ledc_channel_t channel);
    ~Servo();
    void set_angle(int angle);
    void set_min_pulse_width(uint32_t min_pulse_width);
    void set_max_pulse_width(uint32_t max_pulse_width);
    void update();
};
