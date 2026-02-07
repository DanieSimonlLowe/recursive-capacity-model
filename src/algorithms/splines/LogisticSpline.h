#pragma once
#include <Eigen/Dense>

class LogisticSpline {
  public:
    LogisticSpline(const Eigen::VectorXd &xs, const Eigen::VectorXd &ys);

    double predict(double x) const;

    double derivative(double x) const;

  private:
    Eigen::VectorXd parameters;
    Eigen::VectorXd xs;

    int getIndex(double x) const;
};
