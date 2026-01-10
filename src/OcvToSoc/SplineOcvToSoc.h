#pragma once
#include "OcvToSoc/OcvToSocBase.h"
#include <cstddef>

template <typename T> class SplineOcvToSoc : public OcvToSocBase {
  public:
    SplineOcvToSoc(const OcvSocData &data, const Eigen::VectorXd &params = Eigen::VectorXd());

    double getSoc(double ocv) override;

    static size_t getParamsCount() { return 0; };
    static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
    static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };

  private:
    T *spline;
};

#include "OcvToSoc/SplineOcvToSoc.tpp"
