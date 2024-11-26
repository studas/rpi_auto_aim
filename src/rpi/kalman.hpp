#ifndef KALMAN_HPP
#define KALMAN_HPP

#include <Eigen/Dense> // For matrix operations
#include <vector>

class KalmanFilter {
public:
    KalmanFilter(double dt, double u_x, double u_y, double std_acc, double x_std_meas, double y_std_meas);
    
    Eigen::Vector2d predict();
    Eigen::Vector2d update(const Eigen::Vector2d& z);

private:
    // Time step
    double dt;

    // State vector [x, y, vx, vy]
    Eigen::VectorXd x;

    // Control input [ax, ay]
    Eigen::Vector2d u;

    // State transition matrix
    Eigen::MatrixXd A;

    // Control input matrix
    Eigen::MatrixXd B;

    // Measurement mapping matrix
    Eigen::MatrixXd H;

    // Process noise covariance
    Eigen::MatrixXd Q;

    // Measurement noise covariance
    Eigen::MatrixXd R;

    // Error covariance
    Eigen::MatrixXd P;
};

#endif // KALMAN_HPP
