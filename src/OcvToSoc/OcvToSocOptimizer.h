#pragma once
#include "Loading/ReadOcvSocCSV.h"
#include "OcvToSoc/OcvToSocBase.h"
#include <bayesopt/bayesopt.hpp>
#include <cstddef>
#include <type_traits>

template <typename T> class OcvToSocOptimizer {
public:
  OcvToSocOptimizer();

  void display(const Eigen::VectorXd &params = Eigen::VectorXd());
  double getObjectiveValue(const Eigen::VectorXd &params = Eigen::VectorXd());
  void optimize();

private:
  T *makeModel(const OcvSocData &data, const Eigen::VectorXd &params);

  OcvSocData train;      // for training the model
  OcvSocData validation; // for tuning and monitoring loss/metrics
  OcvSocData test;       // final unbiased evaluation
};

#include "OcvToSoc/OcvToSocOptimizer.tpp"

