#include "interpolation/hermiteSplineInterpolator.h"

HermiteSplineInterpolator::HermiteSplineInterpolator(double ts) : 
     InterpolatorBase(),
    counter(0),
    t0(0.0), t1(0.0),
    y0(0.0), y1(0.0),
    m0(0.0), m1(0.0) {}

void HermiteSplineInterpolator::update(double measurement, double time) {
    m0 = m1;
    t0 = t1;
    y0 = y1;
    t1 = time;
    y1 = measurement;
    m1 = (y1 - y0) / (t1- t0);
    counter++;
}

double HermiteSplineInterpolator::predict(double time) {
    double h = t1 - t0;
    double s = (time - t0) / h;

    // Hermite basis using fast arithmetic (avoid std::pow for cubes)
    const double s2 = s * s;
    const double s3 = s2 * s;

    const double h00 = 2.0 * s3 - 3.0 * s2 + 1.0;
    const double h10 = s3 - 2.0 * s2 + s;
    const double h01 = -2.0 * s3 + 3.0 * s2;
    const double h11 = s3 - s2;

    // Note: m0 and m1 are time-derivatives (dy/dt), so multiply by h to convert
    return h00 * y0 + h10 * h * m0 + h01 * y1 + h11 * h * m1;
}

bool HermiteSplineInterpolator::canPredict() {
    return counter <= 3;
}

void HermiteSplineInterpolator::reset() {
    counter = 0;
}

HermiteSplineInterpolator* HermiteSplineInterpolator::clone() {
    return new HermiteSplineInterpolator(*this);
}