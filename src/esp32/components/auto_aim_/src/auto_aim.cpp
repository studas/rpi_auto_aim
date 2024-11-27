#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "auto_aim.hpp"

//auto aim rtos task
void auto_aim(void *parameters){

    TaskParameters* params = (TaskParameters*)parameters;

    Coordinate target_error = {0.0, 0.0};

    int aim_freq = 40;
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000 / aim_freq));

        if((*params->auto_aim_state) == DISABLED) continue;

        double q0 = params->x_controller->getCoefficients()[0];

        double x_control = params->x_controller->calculateControl(params->error->x);
        double y_control = params->y_controller->calculateControl(params->error->y);

        int yaw_angle = (x_control + q0)/(2*q0)*180;
        int pitch_angle = (y_control + q0)/(2*q0)*180;

        params->servos[0].set_angle(yaw_angle);
        params->servos[1].set_angle(pitch_angle);
    }
}