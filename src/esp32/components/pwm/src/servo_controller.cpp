#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "servo_controller.hpp"

static const char *TAG = "servo_controller";

#define SERVO_MAX_DEGREE 180           // Maximum angle of rotation
#define LEDC_FREQUENCY 50              // Frequency in Hertz (typical for servos)
#define LEDC_TIMER LEDC_TIMER_0        // Timer number
#if SOC_LEDC_SUPPORT_HS_MODE
    #define LEDC_MODE LEDC_HIGH_SPEED_MODE  // LEDC mode
#else
    #define LEDC_MODE LEDC_LOW_SPEED_MODE   // LEDC mode
#endif

Servo::Servo(int gpio_pin, ledc_channel_t channel) 
    : gpio_pin(gpio_pin), channel(channel){
    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = gpio_pin,
        .speed_mode = LEDC_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    // Set default pulse width range
    this->min_pulse_width = 1000;
    this->max_pulse_width = 2000;
}

Servo::~Servo() {
    // Clean up LEDC configuration
    ledc_stop(LEDC_MODE, channel, 0);
}

void Servo::set_angle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > SERVO_MAX_DEGREE) angle = SERVO_MAX_DEGREE;

    this->pulse_width = min_pulse_width +
                           (angle) * ((max_pulse_width - min_pulse_width) / SERVO_MAX_DEGREE);
    this->update();
    ESP_LOGI(TAG, "Setting servo %d to %d degrees", gpio_pin, angle);
}

void Servo::add_pulse_width(uint32_t change) {
    if (change == 0) return;

    this->pulse_width += change;
    if (this->pulse_width < min_pulse_width) this->pulse_width = this->min_pulse_width;
    if (this->pulse_width > max_pulse_width) this->pulse_width = this->max_pulse_width;
    this->update();

    if(!(this->pulse_width == this->min_pulse_width) && !(this->pulse_width == this->max_pulse_width)) {
        ESP_LOGI(TAG, "Adding %ld to servo %d. New pulse width %ld", change, this->gpio_pin, this->pulse_width);
    }
}

void Servo::set_min_pulse_width(uint32_t min_pulse_width) {
    this->min_pulse_width = min_pulse_width;
    this->pulse_width = this->min_pulse_width;
    this->update();
    ESP_LOGI(TAG, "Setting min pulse width of %d to %ld", this->gpio_pin, this->min_pulse_width);
}

void Servo::set_max_pulse_width(uint32_t max_pulse_width) {
    this->max_pulse_width = max_pulse_width;
    this->pulse_width = this->max_pulse_width;
    this->update();
    ESP_LOGI(TAG, "Setting max pulse width of %d to %ld", this->gpio_pin, this->max_pulse_width);
}

void Servo::update() {
    uint32_t duty = (this->pulse_width * 8192) / 20000;
    ledc_set_duty(LEDC_MODE, channel, duty);
    ledc_update_duty(LEDC_MODE, channel);
}
