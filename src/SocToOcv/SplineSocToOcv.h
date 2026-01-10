#pragma once
#include "SocToOcv/SocToOcvBase.h"
#include <cstddef>

template <typename T> class SplineSocToOcv : public SocToOcvBase {
  public:
    SplineSocToOcv(const OcvSocData &data, const Eigen::VectorXd &params = Eigen::VectorXd());

    double getOcv(double soc) override;
    double getOcvToSocDerivative(double soc) override;

    static size_t getParamsCount() { return 0; };
    static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
    static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };

  private:
    T *spline;
};

#include "SocToOcv/SplineSocToOcv.tpp"
