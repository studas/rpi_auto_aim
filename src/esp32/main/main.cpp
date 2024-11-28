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
    servo0.set_min_pulse_width(1300);
    servo0.set_max_pulse_width(1800);
    Servo servo1(12, LEDC_CHANNEL_1);
    servo1.set_min_pulse_width(1300);
    servo1.set_max_pulse_width(1800);
    Servo servos[] = {servo0, servo1};

    AutoAimState auto_aim_state = DISABLED;

    I2CSlave i2c_slave(21, 22, 0x0A, 0);

    double kp = 0.320;
    double ki = 0.015;
    double kd = 0.051;
    double step = 0.2;
    ParametricController x_controller({kp, ki, kd});
    ParametricController y_controller({kp, ki, kd});

    Coordinate error = {0.0, 0.0};
    
    while(1){
        char *received_data = i2c_slave.read_buffer();

        if (received_data == NULL) continue;

        int command_count = 0;
        char **commands = divideI2CPacket(received_data, &command_count);

        for (int j = 0; j < command_count; j++) {
            char *command = commands[j];
            RpiDataPacket *numbers = parseServoCommand(command);

            if (numbers == NULL) continue;
            bool valid_servo_id = numbers->param1 >= 0 && numbers->param1 < sizeof(servos) / sizeof(Servo);

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
                {
                    error.x = numbers->param1;
                    error.y = numbers->param2;

                    if (auto_aim_state == DISABLED) break;
                    //ESP_LOGI("main", "Error: (%f, %f)", error.x, error.y);
                    double x_control = x_controller.calculateControl(error.x);
                    double y_control = y_controller.calculateControl(error.y);

                    servos[0].add_pulse_width(x_control*step);
                    servos[1].add_pulse_width(y_control*step);
                    break;
                }
                
                case SET_CONTROLLER_PARAMETERS:

                    if(numbers->param1 == 0) kp = numbers->param2/1000.0f;
                    else if(numbers->param1 == 1) ki = numbers->param2/1000.0f;
                    else if(numbers->param1 == 2) kd = numbers->param2/1000.0f;
                    else if(numbers->param1 == 3) step = numbers->param2/10.0f;
                    else if(numbers->param1 == 4){
                        x_controller.reset();
                        y_controller.reset();
                        ESP_LOGI("main", "Controller reset");
                    }
                    else{
                        ESP_LOGE("main", "Invalid controller parameter");
                        break;
                    }

                    ESP_LOGI("main", "New controller parameters: kp=%f, ki=%f, kd=%f, step: %f", kp, ki, kd, step);

                    x_controller.updateCoefficients({kp, ki, kd});
                    y_controller.updateCoefficients({kp, ki, kd});
                    break;
                
                case TOGGLE_AUTO_AIM:
                    if (numbers->param2 < 0 || numbers->param2 > 3) {
                        ESP_LOGE("main", "Invalid auto aim state");
                        break;
                    }
                    auto_aim_state = static_cast<AutoAimState>(numbers->param2);
                    ESP_LOGI("main", "Auto aim: %s", auto_aim_state_strings[auto_aim_state]);
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
