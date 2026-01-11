#include "SocToOcv/SocToOcvBase.h"
#include <cmath>

template <typename T> class LogisticSplineSocToOcv : SocToOcvBase {
  public:
    LogisticSplineSocToOcv(const OcvSocData &data, const Eigen::VectorXd &params);

    double getOcv(double soc) override;
    double getOcvToSocDerivative(double soc) override;

    static size_t getParamsCount() { return 0; };
    static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
    static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };

  private:
    T *spline;
};

#include "SocToOcv/LogisticSplineSocToOcv.tpp"
