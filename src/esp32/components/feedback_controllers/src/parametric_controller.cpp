#include "esp_log.h"

#include "parametric_controller.hpp"

static const char *TAG = "parametric_controller";

ParametricController::ParametricController(const std::vector<double>& q_coeffs)
    : q(q_coeffs), prev_u(0.0) {
    if (q.empty()) {
        ESP_LOGI(TAG, "No coefficients provided");
        return;
    }

    prev_error.resize(q.size(), 0.0);
}

double ParametricController::calculateControl(double desired, double current) {
    double error = desired - current;

    double u = prev_u;
    for (std::size_t j = 0; j < q.size(); ++j) {
        u += q[j] * prev_error[j];
    }
    for (std::size_t j = prev_error.size() - 1; j > 0; --j) {
        prev_error[j] = prev_error[j - 1];
    }
    prev_error[0] = error;
    prev_u = u;

    return u;
}

void ParametricController::updateCoefficients(const std::vector<double>& q_coeffs) {
    if (q_coeffs.empty()) {
        ESP_LOGI(TAG, "No coefficients provided");
        return;
    }
    q = q_coeffs;
    prev_error.resize(q.size(), 0.0);
    prev_u = 0.0;
}

void ParametricController::updateCoefficients(double kp, double ki, double kd) {
    double q0 = kp*kd;
    double q1 = kp*(ki-2*kd+1);
    double q2 = kp*(kd-1);
    updateCoefficients({q0, q1, q2});
}

