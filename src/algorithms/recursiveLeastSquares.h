#pragma once
#include <Eigen/Dense>

class RecursiveLeastSquares {
    public:
        RecursiveLeastSquares(size_t dimension, double forgettingFactor, double initialCovariance);
        virtual void update(Eigen::VectorXd x, double y);
        const Eigen::VectorXd getState();
        const Eigen::MatrixXd getCovariance();
        void multCovariance(double mult);

        virtual void setParams(Eigen::VectorXd& params); 
        virtual size_t getParamsCount();
        virtual Eigen::VectorXd getLowerBounds();
        virtual Eigen::VectorXd getUpperBounds();
        virtual RecursiveLeastSquares* clone();

    protected:
        Eigen::MatrixXd covariance;
        Eigen::VectorXd state;
        double forgettingFactor;
        double initialCovariance;
        size_t dimension;
};