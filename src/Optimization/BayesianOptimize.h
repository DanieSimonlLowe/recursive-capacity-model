#pragma once
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "Loading/ProcessNasaCycles.h"



template<typename BatteryModel>
Eigen::VectorXd BayesianOptimize(
    double capacity,
    ErrorMetric metric,
    bool useMeasuredCapacity=false
);

#include "Optimization/BayesianOptimize.tpp"