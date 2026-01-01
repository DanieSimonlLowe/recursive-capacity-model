#pragma once

#include <Eigen/Dense>
#include "CapacityEstimation/CapacityEstimatorBase.h"

template<typename  RLS>
class RlsCapacityEstimator : public CapacityEstimatorBase {
    public:
        RlsCapacityEstimator(const double capacity,const Eigen::VectorXd& params);

        void update(double current, double deltaTime, double diffSoc) override;

        double const getCapacity() override;

        double const getCapacityVariance();

        bool const canCalculateStateCapacity() override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();

    private:
        RLS model;
        double maxRelativeVariance;
};

#include "CapacityEstimation/RlsCapacityEstimator.tpp"
