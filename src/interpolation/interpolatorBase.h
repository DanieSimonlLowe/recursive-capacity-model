#pragma once
#include <vector>
#include <stdexcept>
#include <Eigen/Dense>

class InterpolatorBase {
    
    public:
        virtual ~InterpolatorBase() = default;
    
        virtual double predict(double time) = 0;
        virtual void update(double measurement, double time) = 0;
        virtual bool canPredict() = 0;
        virtual void reset() = 0;
        // virtual size_t getParamsCount();
        // virtual Eigen::VectorXd getLowerBounds();
        // virtual Eigen::VectorXd getUpperBounds();
};


