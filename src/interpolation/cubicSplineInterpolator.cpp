
#include "interpolation/cubicSplineInterpolator.h"


CubicSplineInterpolator::CubicSplineInterpolator(double ts, unsigned int windowSize) :
     InterpolatorBase(),
    windowSize(windowSize) {
    times = std::vector<double>();
    measurements = std::vector<double>();
};

void CubicSplineInterpolator::update(double measurement, double time) {
    times.push_back(time);
    measurements.push_back(measurement);
    if (times.size() > windowSize) {
        times.erase(times.begin());
        measurements.erase(measurements.begin());
    }
}

CubicSplineInterpolator* CubicSplineInterpolator::clone() {
    return new CubicSplineInterpolator(*this);
}

double CubicSplineInterpolator::predict(double time) {
        // Eigen::Map requires contiguous memory and non-zero size
    Eigen::Map<const Eigen::VectorXd> tvec(times.data(), static_cast<int>(times.size()));
    Eigen::Map<const Eigen::VectorXd> yvec(measurements.data(), static_cast<int>(measurements.size()));

    CubicSpline spline(tvec, yvec);
    return spline.predict(time);
}

bool CubicSplineInterpolator::canPredict() {
    return times.size() > 5;
}

void CubicSplineInterpolator::reset() {
    times.clear();
    measurements.clear();
}

