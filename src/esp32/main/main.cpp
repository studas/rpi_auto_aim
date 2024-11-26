#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "ServoController.hpp"
#include "i2c_slave.hpp"

typedef struct {
    uint32_t num1;
    uint32_t num2;
    uint32_t num3;
} Numbers;


Numbers *parseServoCommand(const char *str) {
    Numbers *result = (Numbers*)malloc(sizeof(Numbers));
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
        if (count == 0) result->num1 = num;
        else if (count == 1) result->num2 = num;
        else if (count == 2) result->num3 = num;

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
        
        if (received_data == NULL) {
            continue;
        }

        Numbers *numbers = parseServoCommand((char *)received_data);

        if (numbers == NULL) continue;

        switch (numbers->num1){
        case 0:
            servos[numbers->num2].set_angle(numbers->num3);
            break;

        case 1:
            servos[numbers->num2].set_min_pulse_width(numbers->num3);
            break;

        case 2:
            servos[numbers->num2].set_max_pulse_width(numbers->num3);
            break;
        
        default:
            printf("Invalid command\n");
            break;
        }

        free(received_data);
        free(numbers);
    }

}