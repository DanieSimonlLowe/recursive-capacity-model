#pragma once

#include "SocToOcv/SocToOcvBase.h"
#include <vector>

class CapacityEstimatorBase {
  public:
    ~CapacityEstimatorBase() = default;

    virtual void update(double currentArea, double diffSoc) = 0;

    virtual double const getCapacity() = 0;

    virtual bool const canCalculateStateCapacity() = 0;

    // static size_t getParamsCount() { return 0; };
    // static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
    // static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};
