#pragma once
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "loading/processNasaCycles.h"
#include "optimization/HyperparameterOptimizable.h"


Eigen::VectorXd bayesianOptimize(
    HyperparameterOptimizable& optimizable
);