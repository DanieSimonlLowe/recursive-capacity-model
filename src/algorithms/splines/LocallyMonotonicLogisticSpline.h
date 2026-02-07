#pragma once
#include <Eigen/Dense>

class LocallyMonotonicLogisticSpline {
  public:
    LocallyMonotonicLogisticSpline(const Eigen::VectorXd &xs, const Eigen::VectorXd &ys,
                                   const double tolerance, const double valueWeightMult,
                                   const double derivativeWeight);

    double predict(double x) const;

    double derivative(double x) const;

  private:
    Eigen::VectorXd parameters;
    Eigen::VectorXd xs;

    int getIndex(double x) const;
};
