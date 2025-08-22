#pragma once
#include <Eigen/Dense>

class CubicSpline {
    public:
        CubicSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y);

        double predict(double time);
    private:
        Eigen::VectorXd M;
        Eigen::VectorXd y;
        Eigen::VectorXd t;
};