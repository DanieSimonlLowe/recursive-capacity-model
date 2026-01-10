#pragma once
#include "Loading/ReadOcvSocCsv.h"
#include <Eigen/Dense>

class SocToOcvBase {
  public:
    ~SocToOcvBase() = default;

    virtual double getOcv(double soc) = 0;
    virtual double getOcvToSocDerivative(double soc) = 0;

    // virtual size_t getParamsCount() { return 0; };
    // virtual const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
    // virtual const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};
