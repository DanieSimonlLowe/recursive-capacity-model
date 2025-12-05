#include "Algorithms/ExtendedKalmanFilter.h"

ExtendedKalmanFilter::ExtendedKalmanFilter(
    const Eigen::MatrixXd &processNoise, 
    const Eigen::MatrixXd &measurementNoise,
    const Eigen::VectorXd &initialState,
    const Eigen::MatrixXd &initialCovariance)
    : Q_(processNoise),
      R_(measurementNoise),
      x_(initialState),
      P_(initialCovariance)
{
    const int stateDim = initialState.size();
    // Check Q dimensions
    if (Q_.rows() != stateDim || Q_.cols() != stateDim) {
        throw std::invalid_argument("Process noise covariance Q must be square and match state dimension.");
    }
    // Check P dimensions
    if (P_.rows() != stateDim || P_.cols() != stateDim) {
        throw std::invalid_argument("Initial covariance P must be square and match state dimension.");
    }
    // Check R is square
    if (R_.rows() != R_.cols()) {
        throw std::invalid_argument("Measurement noise covariance R must be square.");
    }
}

void ExtendedKalmanFilter::predict() {
    Eigen::MatrixXd jacobian = predictionJacobian(x_);  // Evaluate at current state
    x_ = predictionFunction(x_);  // Then update state
    P_ = jacobian * P_ * jacobian.transpose() + Q_;
    clampState();
}

void ExtendedKalmanFilter::measure(const Eigen::VectorXd &measurement) {
    const Eigen::MatrixXd H_ = measurementJacobian(x_);
    const Eigen::MatrixXd S_ = H_ * P_ * H_.transpose() + R_;
    const Eigen::MatrixXd k_ = P_ * H_.transpose() * S_.inverse();
    const Eigen::VectorXd z_ = measurementFunction(x_);
    x_ = x_ + k_ * (measurement - z_);

    const Eigen::MatrixXd I = Eigen::MatrixXd::Identity(P_.rows(), P_.cols());
    const Eigen::MatrixXd temp = (I - k_ * H_);
    P_ = temp * P_ * temp.transpose() + k_ * R_ * k_.transpose();
    clampState();
}

const Eigen::VectorXd& ExtendedKalmanFilter::getState() {
    return x_;
}

const Eigen::MatrixXd& ExtendedKalmanFilter::getCovariance() {
    return P_;
}

void ExtendedKalmanFilter::setState(const Eigen::VectorXd &state) {
    x_ = state;
}