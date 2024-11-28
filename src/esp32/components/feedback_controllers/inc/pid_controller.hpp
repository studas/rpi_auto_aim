#ifndef pid_controller_HPP
#define pid_controller_HPP

#include <vector>

enum ControllerParameters{
    KP = 0,
    KI = 1,
    KD = 2,
    KS = 3,
    RESET = 4
};

class PIDController {
public:
    // Constructor to initialize the coefficients
    PIDController(double kp, double ki, double kd, double ks) : kp(kp), ki(ki), kd(kd), ks(ks), prev_ui(0.0), prev_error(0.0) {}

    double calculateControl(double error);
    void reset();
    void updateKp(double kp);
    void updateKi(double ki);
    void updateKd(double kd);
    void updateKs(double ks);

private:
    double kp, ki, kd, ks; // Coefficients
    double prev_ui, prev_error; 
};

#endif // pid_controller_HPP
