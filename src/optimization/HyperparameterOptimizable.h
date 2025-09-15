#pragma once
#include "loading/processNasaCycles.h"
#include <vector>

class HyperparameterOptimizable: public CycleHandler {
    public:
        virtual ~HyperparameterOptimizable() = default;

        virtual void setParams(Eigen::VectorXd& params) = 0; 
        virtual size_t getParamsCount() = 0;
        virtual HyperparameterOptimizable* clone() = 0;
        virtual std::vector<double> getLowerBounds() = 0;
        virtual std::vector<double> getUpperBounds() = 0;
        virtual void display() = 0;
        virtual double getObjectiveValue() = 0;
}