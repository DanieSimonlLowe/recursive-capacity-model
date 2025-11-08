#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <vector>
#include <stack>
#include "Algorithms/splines/PolynomialSpline.h"



class MonotoneQuinticSpline {
    public:
        MonotoneQuinticSpline(Eigen::VectorXd xs, Eigen::VectorXd ys, int shrinkFactor);

        double predict(double x) const;

        double derivative(double x) const;

    private:
        std::vector<std::array<double, 6>> parameters; 
        Eigen::VectorXd xs;

        size_t getIndex(double x) const;
};   