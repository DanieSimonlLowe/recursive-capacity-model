#include "interpolation/interpolatorBase.h"

std::vector<double> InterpolatorBase::GetTimeSteps(double measurement, double time) {
    if (!hasRun) {
        lastTime = time;
        lastMeasuredTime = time;
        hasRun = true;
    }

    if (time - lastMeasuredTime > 10 * timeStepSize) {
        reset();
    }

    update(measurement,time);

    if (!canPredict()) {
        return {};
    }

    if (time <= lastTime) {
        throw std::logic_error("Time should only ever increase.");
    }

    std::vector<double> predictions;
    while (lastTime < time) {
        predictions.push_back(predict(lastTime));
        lastTime += timeStepSize;
    }
    return predictions;
};

void InterpolatorBase::setParams(Eigen::VectorXd& params) {
    throw std::logic_error("setParams function not yet implemented");
};

Eigen::VectorXd InterpolatorBase::getLowerBounds() {
    throw std::logic_error("getLowerBounds function not yet implemented");
    Eigen::VectorXd out(0);
    return out;
};

Eigen::VectorXd InterpolatorBase::getUpperBounds() {
    throw std::logic_error("getUpperBounds function not yet implemented");
    Eigen::VectorXd out(0);
    return out; 
};

size_t InterpolatorBase::getParamsCount() {
    return 0;
};