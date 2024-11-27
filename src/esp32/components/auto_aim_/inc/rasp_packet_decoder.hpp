#ifndef __RASP_PACKET_DECODER_HPP__
#define __RASP_PACKET_DECODER_HPP__

enum ServoCommands {
    SET_ANGLE = 0,
    SET_MIN_PULSE_WIDTH = 1,
    SET_MAX_PULSE_WIDTH = 2,
    SET_ERROR = 3,
    SET_CONTROLLER_PARAMETERS = 4,
    TOGGLE_AUTO_AIM = 5
};

typedef struct {
    ServoCommands command;
    int param1;
    int param2;
} RpiDataPacket;

RpiDataPacket *parseServoCommand(const char *str);
char **divideI2CPacket(const char *str, int *command_count);

#endif