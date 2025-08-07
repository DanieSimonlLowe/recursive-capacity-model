#include "interpolation/twoPointLinearInterpolator.h"

bool TwoPointLinearInterpolator::canPredict() {
    return updatedCount > 1;
}


double TwoPointLinearInterpolator::predict(double time) {
    return measurementA + 
        ((measurementB - measurementA) / (timeB - timeA) ) *
        (time - timeA);
}

void TwoPointLinearInterpolator::update(double measurement, double time) {
    timeA = timeB;
    measurementA = measurementB;

    measurementB = measurement;
    timeB = time;
    
    updatedCount++;
}

InterpolatorBase* TwoPointLinearInterpolator::clone() {
    return new TwoPointLinearInterpolator(*this);
}

void TwoPointLinearInterpolator::reset() {
    updatedCount = 0;
}

TwoPointLinearInterpolator::TwoPointLinearInterpolator(double ts) {
    timeStepSize = ts;
}