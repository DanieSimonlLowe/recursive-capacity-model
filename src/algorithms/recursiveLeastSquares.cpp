#include "algorithms/recursiveLeastSquares.h"

RecursiveLeastSquares::RecursiveLeastSquares(size_t dimension, double forgettingFactor, double initialCovariance) {
    if (dimension <= 0) {
        throw std::invalid_argument("dimension can't be 0");
    }

    if (initialCovariance <= 0) {
        throw std::invalid_argument("initialCovariance can't be 0");
    }

    covariance = Eigen::MatrixXd::Identity(dimension,dimension) * initialCovariance;
    state = Eigen::VectorXd::Zero(dimension);
    if (forgettingFactor <= 0 || forgettingFactor > 1) {
        throw std::invalid_argument("forgettingFactor value can't out of the range 0 to 1");
    }
    this->forgettingFactor = forgettingFactor;
}

void RecursiveLeastSquares::update(Eigen::VectorXd x, double y) {
    double spread = x.transpose() * covariance * x;
    
    if (forgettingFactor + spread == 0) {
        return;
    }

    Eigen::VectorXd gain = covariance * x / (forgettingFactor + spread);
    
    
    double estimate = x.transpose() * state;
    double error = y - estimate;
    
    state = state + gain * error;
    
    covariance = (covariance - gain * x.transpose() * covariance) / forgettingFactor;
}

const Eigen::VectorXd RecursiveLeastSquares::getState() {
    return state;
}

const Eigen::MatrixXd RecursiveLeastSquares::getCovariance() {
    return covariance;
}

void RecursiveLeastSquares::multCovariance(double mult) {
    covariance = mult * covariance;
}