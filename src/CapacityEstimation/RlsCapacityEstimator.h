#pragma once

#include <Eigen/Dense>
#include "CapacityEstimation/CapacityEstimatorBase.h"

template<typename  RLS>
class RlsCapacityEstimator : public CapacityEstimatorBase {
    public:
        RlsCapacityEstimator(const Eigen::VectorXd& params);

        void update(double current, double deltaTime, double diffSoc) override;

        double getCapacity() override;

        double getCapacityVariance() override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();

    private:
        RLS model;
};

#include "CapacityEstimation/RlsCapacityEstimator.tpp"