#ifndef PARAMETRIC_CONTROLLER_HPP
#define PARAMETRIC_CONTROLLER_HPP

#include <vector>

class ParametricController {
public:
    // Constructor to initialize the coefficients
    ParametricController(const std::vector<double>& q_coeffs);

    // Compute the next control signal
    double calculateControl(double desired, double current);

    std::vector<double> getCoefficients() const { return q; }
    void updateCoefficients(const std::vector<double>& q_coeffs);

    void updateCoefficients(double kp, double ki, double kd);

private:
    std::vector<double> q; // Coefficients for the error terms
    double prev_u;         // Previous control signal
    std::vector<double> prev_error; // History of previous errors
};

#endif // PARAMETRIC_CONTROLLER_HPP
