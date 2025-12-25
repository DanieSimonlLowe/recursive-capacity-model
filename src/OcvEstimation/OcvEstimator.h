#pragma once
#include <Eigen/Dense>

class OcvEstimator {
    public:
        virtual void update(const Eigen::VectorXd& current, 
                        const Eigen::VectorXd& voltage) = 0;
        virtual void setDeltaTime(double deltaTime) = 0;

        virtual double getOcv() = 0;
        virtual bool canCalculateState() = 0;
};