#include "kalman.hpp"

KalmanFilter::KalmanFilter(double dt, double u_x, double u_y, double std_acc, double x_std_meas, double y_std_meas)
    : dt(dt), u(u_x, u_y) {
    // Initialize state vector
    x = Eigen::VectorXd(4);
    x << 0, 0, 0, 0;

    // State transition matrix
    A = Eigen::MatrixXd(4, 4);
    A << 1, 0, dt, 0,
         0, 1, 0, dt,
         0, 0, 1,  0,
         0, 0, 0,  1;

    // Control input matrix
    B = Eigen::MatrixXd(4, 2);
    B << dt * dt / 2, 0,
         0, dt * dt / 2,
         dt, 0,
         0, dt;

    // Measurement mapping matrix
    H = Eigen::MatrixXd(2, 4);
    H << 1, 0, 0, 0,
         0, 1, 0, 0;

    // Process noise covariance
    Q = Eigen::MatrixXd(4, 4);
    Q << (dt * dt * dt * dt) / 4, 0, (dt * dt * dt) / 2, 0,
         0, (dt * dt * dt * dt) / 4, 0, (dt * dt * dt) / 2,
         (dt * dt * dt) / 2, 0, (dt * dt), 0,
         0, (dt * dt * dt) / 2, 0, (dt * dt);
    Q *= std_acc * std_acc;

    // Measurement noise covariance
    R = Eigen::MatrixXd(2, 2);
    R << x_std_meas * x_std_meas, 0,
         0, y_std_meas * y_std_meas;

    // Error covariance matrix
    P = Eigen::MatrixXd::Identity(4, 4);
}

Eigen::Vector2d KalmanFilter::predict() {
    // Predict state
    x = A * x + B * u;

    // Predict error covariance
    P = A * P * A.transpose() + Q;

    // Return position only (x, y)
    return x.head<2>();
}

Eigen::Vector2d KalmanFilter::update(const Eigen::Vector2d& z) {
    // Compute Kalman Gain
    Eigen::MatrixXd S = H * P * H.transpose() + R;
    Eigen::MatrixXd K = P * H.transpose() * S.inverse();

    // Update state
    x = x + K * (z - H * x);

    // Update error covariance
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(P.rows(), P.cols());
    P = (I - K * H) * P;

    // Return updated position (x, y)
    return x.head<2>();
}

