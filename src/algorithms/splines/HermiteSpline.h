#pragma once
#include <Eigen/Dense>

class HermiteSpline {
public:
    // Constructor: xs and ys are data points, derivatives can be auto-computed or provided
    HermiteSpline(Eigen::VectorXd xs, Eigen::VectorXd ys);
    HermiteSpline(Eigen::VectorXd xs, Eigen::VectorXd ys, Eigen::VectorXd derivatives);
    
    double predict(double x) const;
    double derivative(double x) const;

private:
    Eigen::VectorXd xs;
    Eigen::VectorXd ys;
    Eigen::VectorXd ms;  // derivatives at each point
    
    // Hermite polynomial coefficients for each segment
    Eigen::VectorXd c0s;  // constant terms
    Eigen::VectorXd c1s;  // linear terms
    Eigen::VectorXd c2s;  // quadratic terms
    Eigen::VectorXd c3s;  // cubic terms
    
    size_t getIndex(double x) const;
    void computeDerivatives();
    void computeCoefficients();
};