
#include "Interpolation/CubicSplineInterpolator.h"


CubicSplineInterpolator::CubicSplineInterpolator(Eigen::VectorXd params) :
     InterpolatorBase() {
};

void CubicSplineInterpolator::update(double measurement, double time) {
    times.push_back(time);
    measurements.push_back(measurement);
}

double CubicSplineInterpolator::predict(double time) {
    // Eigen::Map requires contiguous memory and non-zero size
   if (!spline) {
        Eigen::Map<const Eigen::VectorXd> tvec(times.data(), static_cast<int>(times.size()));
        Eigen::Map<const Eigen::VectorXd> yvec(measurements.data(), static_cast<int>(measurements.size()));
        spline = std::make_unique<CubicSpline>(tvec, yvec);
        double last_m = measurements.back();
        double last_t = times.back();
        measurements.clear();
        times.clear();
        measurements.push_back(last_m);
        times.push_back(last_t);
    }
    return spline->predict(time);
}

bool CubicSplineInterpolator::canPredict() {
    return times.size() > 5;
}

void CubicSplineInterpolator::reset() {    
    spline.reset(); 
    measurements.clear();
    times.clear();
}


size_t CubicSplineInterpolator::getParamsCount() {
    return 0;
}

Eigen::VectorXd CubicSplineInterpolator::getLowerBounds() {
    return Eigen::VectorXd::Ones(0);
}

Eigen::VectorXd CubicSplineInterpolator::getUpperBounds() {
    return Eigen::VectorXd::Ones(0);
}