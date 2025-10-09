#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

class PolynomialSpline {
    public:
        PolynomialSpline(const Eigen::VectorXd& xs, const Eigen::VectorXd& ys, unsigned int power);

        double predict(double x);

        double derivative(double x, int order);

    private:
        Eigen::VectorXd parameters;
        Eigen::VectorXd xs;
        unsigned int power;

        size_t getIndex(double x) const;
};