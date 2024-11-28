#include <stdio.h>

#include "esp_log.h"

#include "pid_controller.hpp"

static const char *TAG = "pid_controller";

double PIDController::calculateControl(double error) {
    double up = kp * error;
    double ui = prev_ui + kp * ki * prev_error;
    double ud = kp * kd * (error - prev_error);
    double u = ks*(up + ui + ud);

    prev_error = error;
    prev_ui = ui;

    return u;
}

void PIDController::reset() {
    prev_ui = 0.0;
    prev_error = 0.0;
    ESP_LOGI(TAG, "Controller reset");
}

void PIDController::updateKp(double kp) {
    this->kp = kp;
    ESP_LOGI(TAG, "Updated kp: %f", kp);
}

void PIDController::updateKi(double ki) {
    this->ki = ki;
    ESP_LOGI(TAG, "Updated ki: %f", ki);
}

void PIDController::updateKd(double kd) {
    this->kd = kd;
    ESP_LOGI(TAG, "Updated kd: %f", kd);
}

void PIDController::updateKs(double ks) {
    this->ks = ks;
    ESP_LOGI(TAG, "Updated ks: %f", ks);
}
