#pragma once
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "Loading/ProcessNasaCycles.h"

#include "BatteryModel.h"

template<typename BatteryModel>
Eigen::VectorXd BayesianOptimize(
    double capacity,
    ErrorMetric metric
);

#include "Optimization/BayesianOptimize.tpp"