#pragma once
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "Loading/ProcessNasaCycles.h"
#include "Optimization/HyperparameterOptimizable.h"


Eigen::VectorXd bayesianOptimize(
    HyperparameterOptimizable& optimizable
);