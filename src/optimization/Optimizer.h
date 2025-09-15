#pragma once
#include "loading/processNasaCycles.h"
#include "optimization/HyperparameterOptimizable.h"
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>
#include <cmath>

Eigen::VectorXd bayesianOptimize(
    HyperparameterOptimizable& optimizable
);