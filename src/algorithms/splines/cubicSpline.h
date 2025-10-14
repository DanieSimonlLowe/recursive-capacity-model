#pragma once
#include <Eigen/Dense>

class CubicSpline {
    public:
        CubicSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y);

        double predict(double time);

        double derivative(double time);

        
    private:
        Eigen::VectorXd M;
        Eigen::VectorXd y;
        Eigen::VectorXd t;

        Eigen::VectorXd solveTridiagonal(
            const Eigen::VectorXd& lower,  // sub-diagonal
            const Eigen::VectorXd& diag,   // diagonal  
            const Eigen::VectorXd& upper,  // super-diagonal
            const Eigen::VectorXd& rhs);
};