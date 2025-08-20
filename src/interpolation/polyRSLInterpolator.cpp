#include "interpolation/polyRSLInterpolator.h"

PolyRSLInterpolator::PolyRSLInterpolator(double ts, unsigned int polyDegree, double initialVariance, 
    double forgettingFactor, double resetVarianceMult)
{
    timeStepSize = ts;
    this->polyDegree = polyDegree;
    rsl = new RecursiveLeastSquares(polyDegree + 1, forgettingFactor, initialVariance);
    updateCount = 0;
    this->resetVarianceMult = resetVarianceMult;
}

PolyRSLInterpolator* PolyRSLInterpolator::clone() {
    return new PolyRSLInterpolator(*this);
}

Eigen::VectorXd PolyRSLInterpolator::getVector(double time) {
    Eigen::VectorXd out = Eigen::VectorXd(this->polyDegree + 1);
    for (int i = 0; i <= polyDegree; i++) {
        out(i) = std::pow(time,i);
    }
    return out;
}

double PolyRSLInterpolator::predict(double time) {
    double value = this->getVector(time).dot(this->rsl->getState());
    if (std::isnan(value)) {
        return 0;
    }
    return value;
}

void PolyRSLInterpolator::update(double measurement, double time) {
    this->rsl->update(this->getVector(time),measurement);
    updateCount++;
}

bool PolyRSLInterpolator::canPredict() {
    return updateCount >= (polyDegree + 1) * 3;
}

void PolyRSLInterpolator::reset() {
    updateCount = 0;
    rsl->multCovariance(resetVarianceMult);
}

void PolyRSLInterpolator::setParams(Eigen::VectorXd& params) {
    this->polyDegree = params(0);
    rsl = new RecursiveLeastSquares(polyDegree + 1, 
        1 - 1/params(1),
        pow(10,params(2)) // To make the space log search
        );
    this->resetVarianceMult = pow(10,params(3));

    updateCount = 0;
}

size_t PolyRSLInterpolator::getParamsCount() {
    return 4;
}

Eigen::VectorXd PolyRSLInterpolator::getLowerBounds() {
    Eigen::VectorXd lower(4);
    lower[0] = 1.0;
    lower[1] = 2;
    lower[2] = 1;
    lower[3] = 0;
    
    return lower;
}

Eigen::VectorXd PolyRSLInterpolator::getUpperBounds() {
    Eigen::VectorXd upper(4);
    upper[0] = 6.0;
    upper[1] = 1000;
    upper[2] = 15;
    upper[3] = 6;
    
    return upper;
}
