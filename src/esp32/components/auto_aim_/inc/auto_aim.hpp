#ifndef __AUTO_AIM_HPP__
#define __AUTO_AIM_HPP__

#include "pid_controller.hpp"
#include "servo_controller.hpp"

typedef struct {
    int x;
    int y;
} Coordinate;

enum AutoAimState {
    DISABLED = 0,
    ENABLED = 1,
    OVERRIDE = 2,
};

char ** auto_aim_state_strings = (char*[]) {
    "DISABLED",
    "ENABLED",
    "OVERRIDE",
};

typedef struct {
    Servo* servos;
    AutoAimState* auto_aim_state;
    PIDController* x_controller;
    PIDController* y_controller;
    Coordinate* error;
} TaskParameters;

#endif // __AUTO_AIM_HPP__