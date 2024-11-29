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
#include "pid_controller.hpp"
#include "auto_aim.hpp"
#include "rasp_packet_decoder.hpp"
#include "gpio_t.hpp"

extern "C" void app_main(void){
    Servo servo0(13, LEDC_CHANNEL_0);
    servo0.set_min_pulse_width(1300);
    servo0.set_max_pulse_width(1800);
    Servo servo1(12, LEDC_CHANNEL_1);
    servo1.set_min_pulse_width(1300);
    servo1.set_max_pulse_width(1800);
    Servo servos[] = {servo0, servo1};

    AutoAimState auto_aim_state = MANUAL;

    I2CSlave i2c_slave(21, 22, 0x0A, 0);

    double kp = 0.140;
    double ki = 0.005;
    double kd = 0.062;
    double ks = 0.200;
    PIDController x_controller(kp, ki, kd, ks);
    PIDController y_controller(kp, ki, kd, ks);
    
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
                    x_controller.reset();
                    y_controller.reset();
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
                    if (auto_aim_state == MANUAL) break;
                    Coordinate error = {numbers->param1, numbers->param2};
                    if (error.x == 0 && error.y == 0) {
                        x_controller.reset();
                        y_controller.reset();
                        break;
                    }

                    double x_control = x_controller.calculateControl(error.x);
                    double y_control = y_controller.calculateControl(error.y);
                    ESP_LOGI("main", "Error: (%d, %d) | Control: (%lf, %lf)", error.x, error.y, x_control, y_control);

                    servos[0].add_pulse_width(x_control);
                    servos[1].add_pulse_width(y_control);
                    break;
                }
                
                case SET_CONTROLLER_PARAMETERS:{
                    ControllerParameters param = static_cast<ControllerParameters>(numbers->param1);
                    double value = numbers->param2/1000.0f;
                    switch(param){
                    case KP:
                        x_controller.updateKp(value);
                        y_controller.updateKp(value);
                        break;
                    
                    case KI:
                        x_controller.updateKi(value);
                        y_controller.updateKi(value);
                        break;

                    case KD:
                        x_controller.updateKd(value);
                        y_controller.updateKd(value);
                        break;

                    case KS:
                        x_controller.updateKs(value);
                        y_controller.updateKs(value);
                        break;

                    case RESET:
                        x_controller.reset();
                        y_controller.reset();
                        break;

                    default:
                        ESP_LOGE("main", "Invalid controller parameter");
                        break;   
                    }
                    break;
                }
                
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
