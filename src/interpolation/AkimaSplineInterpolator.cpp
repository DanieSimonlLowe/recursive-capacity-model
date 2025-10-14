
#include "interpolation/AkimaSplineInterpolator.h"


AkimaSplineInterpolator::AkimaSplineInterpolator(double ts, unsigned int windowSize) :
     InterpolatorBase(),
    windowSize(windowSize) {
    times = std::vector<double>();
    measurements = std::vector<double>();
    spline = nullptr;
};

void AkimaSplineInterpolator::update(double measurement, double time) {
    times.push_back(time);
    measurements.push_back(measurement);
    if (times.size() > windowSize) {
        times.erase(times.begin());
        measurements.erase(measurements.begin());
    }
    spline.reset();
}

AkimaSplineInterpolator* AkimaSplineInterpolator::clone() {
    return new AkimaSplineInterpolator(timeStepSize, windowSize);
}


double AkimaSplineInterpolator::predict(double time) {
    // Eigen::Map requires contiguous memory and non-zero size
   if (!spline) {
        Eigen::Map<const Eigen::VectorXd> tvec(times.data(), static_cast<int>(times.size()));
        Eigen::Map<const Eigen::VectorXd> yvec(measurements.data(), static_cast<int>(measurements.size()));
        spline = std::make_unique<AkimaSpline>(tvec, yvec);
        double last_m = measurements.back();
        double last_t = times.back();
        measurements.clear();
        times.clear();
        measurements.push_back(last_m);
        times.push_back(last_t);
    }
    return spline->predict(time);
}

bool AkimaSplineInterpolator::canPredict() {
    return times.size() > windowSize - 1;
}

void AkimaSplineInterpolator::reset() {
    times.clear();
    measurements.clear();
    spline.reset(); 

}

