#pragma once
#include "interpolation/interpolatorBase.h"
#include "interpolation/interpolatorEvaluator.h"
#include "loading/processNasaCycles.h"
#include <bayesopt/bayesopt.hpp>
#include <Eigen/Dense>

enum class MSEType { Voltage, Current, Temperature };

Eigen::VectorXd bayesianOptimizeMSE(
    InterpolatorBase& interpolator,
    MSEType mseType,
    const Eigen::VectorXd& lowerBounds,
    const Eigen::VectorXd& upperBounds
);