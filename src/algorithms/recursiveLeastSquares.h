#pragma once
#include <Eigen/Dense>

class RecursiveLeastSquares {
    public:
        RecursiveLeastSquares(size_t dimension, double forgettingFactor, double initialCovariance);
        void update(Eigen::VectorXd x, double y);
        const Eigen::VectorXd getState();
        const Eigen::MatrixXd getCovariance();
        void multCovariance(double mult);

    private:
        Eigen::MatrixXd covariance;
        Eigen::VectorXd state;
        double forgettingFactor;
};