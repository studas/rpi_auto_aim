#ifndef __AUTO_AIM_HPP__
#define __AUTO_AIM_HPP__

#include "parametric_controller.hpp"
#include "servo_controller.hpp"

typedef struct {
    double x;
    double y;
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
    ParametricController* x_controller;
    ParametricController* y_controller;
    Coordinate* error;
} TaskParameters;

#endif // __AUTO_AIM_HPP__