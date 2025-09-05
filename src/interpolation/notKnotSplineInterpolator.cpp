
#include "interpolation/NotKnotSplineInterpolator.h"
#include "algorithms/notKnotSpline.h"

NotKnotSplineInterpolator::NotKnotSplineInterpolator(double ts, unsigned int windowSize) :
     InterpolatorBase(),
    windowSize(windowSize) {
    times = std::vector<double>();
    measurements = std::vector<double>();
    spline = nullptr;
};

void NotKnotSplineInterpolator::update(double measurement, double time) {

    times.push_back(time);
    measurements.push_back(measurement);
    if (times.size() > windowSize) {
        times.erase(times.begin());
        measurements.erase(measurements.begin());
    }
    spline.reset();
}

NotKnotSplineInterpolator* NotKnotSplineInterpolator::clone() {
    return new NotKnotSplineInterpolator(0.1,windowSize);
}

double NotKnotSplineInterpolator::predict(double time) {
    // Eigen::Map requires contiguous memory and non-zero size

    //NotKnotSpline spline(tvec, yvec);
    if (!spline) {
        Eigen::Map<const Eigen::VectorXd> tvec(times.data(), static_cast<int>(times.size()));
        Eigen::Map<const Eigen::VectorXd> yvec(measurements.data(), static_cast<int>(measurements.size()));
        spline = std::make_unique<NotKnotSpline>(tvec, yvec);
        double last_m = measurements.back();
        double last_t = times.back();
        measurements.clear();
        times.clear();
        measurements.push_back(last_m);
        times.push_back(last_t);
    }
    return spline->predict(time);
}

bool NotKnotSplineInterpolator::canPredict() {
    return times.size() >= windowSize - 1;
}

void NotKnotSplineInterpolator::reset() {
    times.clear();
    measurements.clear();
    spline.reset(); 
}

