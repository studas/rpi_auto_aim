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

        double x_control = params->x_controller->calculateControl(target_error.x, params->error->x);
        double y_control = params->y_controller->calculateControl(target_error.y, params->error->y);

        int yaw_angle = params->servos[0].get_angle() + x_control;
        int pitch_angle = params->servos[1].get_angle() + y_control;

        params->servos[0].set_angle(yaw_angle);
        params->servos[1].set_angle(pitch_angle);
    }
}