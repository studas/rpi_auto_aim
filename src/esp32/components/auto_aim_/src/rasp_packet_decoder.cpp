#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex>

#include "rasp_packet_decoder.hpp"

RpiDataPacket *parseServoCommand(const char *str) {
    RpiDataPacket *result = (RpiDataPacket*)malloc(sizeof(RpiDataPacket));
    int count = 0;
    char *copy = strdup(str);
    char *token;
    const char *delimiters = " ";
    
    token = strtok(copy, delimiters);
    while (token != NULL) {
        int num = atoi(token);
        if (count == 0) result->command = static_cast<ServoCommands>(num);
        else if (count == 1) result->param1 = num;
        else if (count == 2) result->param2 = num;

        count++;
        token = strtok(NULL, delimiters);
    }

    free(copy);

    return result;
}

char **divideI2CPacket(const char *str, int *count) {
    char **result = (char**)malloc(20 * sizeof(char*));
    char *copy = strdup(str);
    char *token;
    const char *delimiters = "\n";
    //regex for 3 integer numbers separated by spaces
    std::regex pattern("(-?\\d+) (-?\\d+) (-?\\d+)");
    
    token = strtok(copy, delimiters);
    *count = 0;
    while (token != NULL) {
        if (!std::regex_match(token, pattern)) {
            token = strtok(NULL, delimiters);
            continue;
        }
        result[(*count)] = token;
        (*count)++;
        token = strtok(NULL, delimiters);
    }

    return result;
}