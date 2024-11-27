#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "servo_controller.hpp"
#include "i2c_slave.hpp"
#include "parametric_controller.hpp"
#include "auto_aim.hpp"
#include "rasp_packet_decoder.hpp"

extern "C" void app_main(void){
    Servo servo0(13, LEDC_CHANNEL_0);
    Servo servo1(12, LEDC_CHANNEL_1);
    Servo servos[] = {servo0, servo1};

    AutoAimState auto_aim_state = DISABLED;

    I2CSlave i2c_slave(21, 22, 0x0A, 0);

    double kp = 0.01;
    double ki = 0.01;
    double kd = 1.02;
    ParametricController x_controller({1.0, 1.0, 1.0});
    x_controller.updateCoefficients(kp, ki, kd);
    ParametricController y_controller({1.0, 1.0, 1.0});
    y_controller.updateCoefficients(kp, ki, kd);

    Coordinate error = {0.0, 0.0};

    xTaskCreate(auto_aim, "auto_aim", 4096, new TaskParameters{servos, &auto_aim_state, &x_controller, &y_controller, &error}, 5, NULL);
    
    while(1){
        char *received_data = i2c_slave.read_buffer();

        if (received_data == NULL) continue;

        int command_count = 0;
        char **commands = divideI2CPacket(received_data, &command_count);

        for (int j = 0; j < command_count; j++) {
            char *command = commands[j];
            RpiDataPacket *numbers = parseServoCommand(command);
            bool valid_servo_id = numbers->param1 >= 0 && numbers->param1 < sizeof(servos) / sizeof(Servo);

            if (numbers == NULL) continue;

            switch (numbers->command){
                case SET_ANGLE:
                    if (!valid_servo_id) break;
                    if (auto_aim_state == ENABLED) break;
                    
                    servos[numbers->param1].set_angle(numbers->param2);
                    break;

                case SET_MIN_PULSE_WIDTH:
                    if (!valid_servo_id) break;
                    if (auto_aim_state == ENABLED) break;

                    servos[numbers->param1].set_min_pulse_width(numbers->param2);
                    break;

                case SET_MAX_PULSE_WIDTH:
                    if (!valid_servo_id) break;
                    if (auto_aim_state == ENABLED) break;

                    servos[numbers->param1].set_max_pulse_width(numbers->param2);
                    break;

                case SET_ERROR:
                    error.x = numbers->param1;
                    error.y = numbers->param2;
                    ESP_LOGI("main", "Error: (%f, %f)", error.x, error.y);
                    break;
                
                case SET_CONTROLLER_PARAMETERS:

                    if(numbers->param1 == 0) kp = numbers->param2/100.0f;
                    else if(numbers->param1 == 1) ki = numbers->param2/100.0f;
                    else if(numbers->param1 == 2) kd = numbers->param2/100.0f;
                    else{
                        ESP_LOGE("main", "Invalid controller parameter");
                        break;
                    }

                    ESP_LOGI("main", "New controller parameters: kp=%f, ki=%f, kd=%f", kp, ki, kd);

                    x_controller.updateCoefficients(kp, ki, kd);
                    y_controller.updateCoefficients(kp, ki, kd);
                    break;
                
                case TOGGLE_AUTO_AIM:
                    if (numbers->param2 < 0 || numbers->param2 > 3) {
                        ESP_LOGE("main", "Invalid auto aim state");
                        break;
                    }
                    auto_aim_state = static_cast<AutoAimState>(numbers->param2);
                    ESP_LOGI("main", "Auto aim state: %d", auto_aim_state);
                    break;

                default:
                    ESP_LOGE("main", "Invalid command");
                    break;
            }

            free(numbers);
        }

        free(commands);
        free(received_data);
    }
}
