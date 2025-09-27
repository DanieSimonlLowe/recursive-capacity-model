#pragma once
#include <Eigen/Dense>

class AkimaSpline {
    public:
        AkimaSpline(Eigen::VectorXd xs, Eigen::VectorXd ys);

        double predict(double x) const;

        double derivative(double x) const;

    private:
        Eigen::VectorXd xs;
        Eigen::VectorXd ys;
        Eigen::VectorXd c1s;
        Eigen::VectorXd c2s;
        Eigen::VectorXd c3s;

        size_t getIndex(double x) const;

};   