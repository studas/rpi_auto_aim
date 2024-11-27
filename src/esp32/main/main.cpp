#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "ServoController.hpp"
#include "i2c_slave.hpp"

enum ServoCommands {
    SET_ANGLE = 0,
    SET_MIN_PULSE_WIDTH = 1,
    SET_MAX_PULSE_WIDTH = 2,
    SET_CENTROID = 3,
    SET_CONTROLLER_PARAMETERS = 4
};

typedef struct {
    ServoCommands command;
    int param1;
    int param2;
} RpiDataPacket;

RpiDataPacket *parseServoCommand(const char *str) {
    RpiDataPacket *result = (RpiDataPacket*)malloc(sizeof(RpiDataPacket));
    int count = 0;
    char *copy = strdup(str);
    char *token;
    const char *delimiters = " ";
    
    token = strtok(copy, delimiters);
    while (token != NULL) {
        if (!isdigit(*token) && *token != '-' && *token != '+') {
            free(copy);
            free(result);
            return NULL; // Invalid token (not a number)
        }

        int num = atoi(token);
        if (count == 0) result->command = static_cast<ServoCommands>(num);
        else if (count == 1) result->param1 = num;
        else if (count == 2) result->param2 = num;

        count++;
        token = strtok(NULL, delimiters);
    }

    free(copy);

    if (count != 3) {
        free(result);
        return NULL; // Not enough numbers
    }

    return result;
}

extern "C" void app_main(void){
    Servo servo0(13, LEDC_CHANNEL_0);
    Servo servo1(12, LEDC_CHANNEL_1);

    Servo servos[] = {servo0, servo1};

    I2CSlave i2c_slave(21, 22, 0x0A, 0);
    
    while(1){
        char *received_data = i2c_slave.read_buffer();

        if (received_data == NULL) continue;

        char **commands = (char**)malloc(20 * sizeof(char*));
        char *command = strtok(received_data, "\n");
        int command_count = 0;
        while (command != NULL){
            commands[command_count] = command;
            command_count++;
            command = strtok(NULL, "\n");
        }

        for (int j = 0; j < command_count; j++) {
            command = commands[j];
            RpiDataPacket *numbers = parseServoCommand(command);
            bool valid_servo_id = numbers->param1 >= 0 && numbers->param1 < sizeof(servos) / sizeof(Servo);

            if (numbers == NULL) continue;

            switch (numbers->command){
                case SET_ANGLE:
                    if (!valid_servo_id) break;
                    
                    servos[numbers->param1].set_angle(numbers->param2);
                    break;

                case SET_MIN_PULSE_WIDTH:
                    if (!valid_servo_id) break;

                    servos[numbers->param1].set_min_pulse_width(numbers->param2);
                    break;

                case SET_MAX_PULSE_WIDTH:
                    if (!valid_servo_id) break;
                    
                    servos[numbers->param1].set_max_pulse_width(numbers->param2);
                    break;

                case SET_CENTROID:
                    break;
                
                case SET_CONTROLLER_PARAMETERS:
                    break;

                default:
                    printf("Invalid command\n");
                    break;
            }

            free(numbers);
        }

        free(commands);
        free(received_data);
    }
}