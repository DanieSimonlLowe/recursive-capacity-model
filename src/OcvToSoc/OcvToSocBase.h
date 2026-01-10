#pragma once
#include "Loading/ReadOcvSocCsv.h"
#include <Eigen/Dense>

class OcvToSocBase {
  public:
    ~OcvToSocBase() = default;

    virtual double getSoc(double ocv) = 0;

    // virtual size_t getParamsCount() { return 0; };
    // virtual const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd();
    // }; virtual const Eigen::VectorXd getUpperBounds() { return
    // Eigen::VectorXd(); };
};

