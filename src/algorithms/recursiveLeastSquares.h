#pragma once
#include <Eigen/Dense>

class RecursiveLeastSquares {
    public:
        RecursiveLeastSquares(size_t dimension, double forgettingFactor=1, double initialCovariance=1);
        void update(Eigen::VectorXd x, double y);
        const Eigen::VectorXd getState();
        const Eigen::MatrixXd getCovariance();
        void multCovariance(double mult);

        void setParams(Eigen::VectorXd& params); 
        
        static size_t getParamsCount();
        static Eigen::VectorXd getLowerBounds();
        static Eigen::VectorXd getUpperBounds();

    protected:
        Eigen::MatrixXd covariance;
        Eigen::VectorXd state;
        double forgettingFactor;
        double initialCovariance;
        size_t dimension;
};