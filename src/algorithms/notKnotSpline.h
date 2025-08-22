#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

class NotKnotSpline {
    public:
        NotKnotSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y);

        double predict(double time);
    private:
        Eigen::VectorXd M;
        Eigen::VectorXd y;
        Eigen::VectorXd t;
};