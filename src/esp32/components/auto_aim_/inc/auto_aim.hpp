#ifndef __AUTO_AIM_HPP__
#define __AUTO_AIM_HPP__

#include "parametric_controller.hpp"
#include "servo_controller.hpp"

typedef struct {
    double x;
    double y;
} Coordinate;

enum AutoAimState {
    OVERRIDE = 0,
    DISABLED = 1,
    ENABLED = 2,
};

typedef struct {
    Servo* servos;
    AutoAimState* auto_aim_state;
    ParametricController* x_controller;
    ParametricController* y_controller;
    Coordinate* error;
} TaskParameters;

void auto_aim(void* parameters);

#endif // __AUTO_AIM_HPP__