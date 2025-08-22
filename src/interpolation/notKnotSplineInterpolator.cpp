
#include "interpolation/NotKnotSplineInterpolator.h"
#include "algorithms/notKnotSpline.h"

NotKnotSplineInterpolator::NotKnotSplineInterpolator(double ts, unsigned int windowSize) :
     InterpolatorBase(),
    windowSize(windowSize) {
    times = std::vector<double>();
    measurements = std::vector<double>();
};

void NotKnotSplineInterpolator::update(double measurement, double time) {
    times.push_back(time);
    measurements.push_back(measurement);
    if (times.size() > windowSize) {
        times.erase(times.begin());
        measurements.erase(measurements.begin());
    }
}

NotKnotSplineInterpolator* NotKnotSplineInterpolator::clone() {
    return new NotKnotSplineInterpolator(*this);
}

double NotKnotSplineInterpolator::predict(double time) {
        // Eigen::Map requires contiguous memory and non-zero size
    Eigen::Map<const Eigen::VectorXd> tvec(times.data(), static_cast<int>(times.size()));
    Eigen::Map<const Eigen::VectorXd> yvec(measurements.data(), static_cast<int>(measurements.size()));

    NotKnotSpline spline(tvec, yvec);
    return spline.predict(time);
}

bool NotKnotSplineInterpolator::canPredict() {
    return times.size() >= 4;
}

void NotKnotSplineInterpolator::reset() {
    times.clear();
    measurements.clear();
}

