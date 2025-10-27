#pragma once

#include <vector>
#include "socOcv/SocOcvCurveBase.h"

class CapacityEstimatorBase {
    public:
        ~CapacityEstimatorBase() = default;

        virtual void update(double current, double deltaTime, double diffSoc);

        virtual double getCapacity() = 0;

        virtual double getCapacityVariance() = 0;

        //static size_t getParamsCount() { return 0; };
        //static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        //static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};