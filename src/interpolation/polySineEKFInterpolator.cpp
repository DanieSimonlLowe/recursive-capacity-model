#include "interpolation/polySineEKFInterpolator.h"

PolySineEKFInterpolator::PolySineEKFInterpolator(double ts, unsigned int polyDegree, double updateNoise, 
                        double measurementNoise, double initialError, double resetSeccoundsEquivalent) 
{
    timeStepSize = ts;
    this->polyDegree = polyDegree;
    this->resetSeccoundsEquivalent = resetSeccoundsEquivalent;

    Eigen::MatrixXd processNoise = Eigen::MatrixXd::Identity(polyDegree + 3, polyDegree + 3) * updateNoise;
    Eigen::MatrixXd measurementNoiseMatrix = Eigen::MatrixXd::Identity(1, 1) * measurementNoise;
    
    // state a,b,w, n1,...nn
    Eigen::VectorXd initialState = Eigen::VectorXd::Zero(polyDegree + 3);
    initialState(2) = 3;

    Eigen::MatrixXd initialCovariance = Eigen::MatrixXd::Identity(polyDegree + 3, polyDegree + 3) * initialError;

    ekf = new HelperEKF(processNoise, measurementNoiseMatrix, initialState, initialCovariance);
    ekf->polyDegree = polyDegree;
}

PolySineEKFInterpolator* PolySineEKFInterpolator::clone() {
    return new PolySineEKFInterpolator(*this);
}


void PolySineEKFInterpolator::reset() {
    stateCount = 0;

    ekf->time = ekf->lastPredictTime + resetSeccoundsEquivalent;
    ekf->predict();
    ekf->hasPredicted = false;
}

Eigen::MatrixXd measurementJacobianHelper(const Eigen::VectorXd &state, unsigned int polyDegree, double time) {
    Eigen::MatrixXd out(1, polyDegree + 3);

    out(0,0) = std::sin(state(2) * time);
    out(0,1) = std::cos(state(2) * time);

    out(0,2) = time * (state(0) * std::cos(state(2) * time) 
                     - state(1) * std::sin(state(2) * time));

    for (int i = 0; i < polyDegree; i++) {
        out(0,i+3) = std::pow(time, i);
    }

    return out;
}


bool PolySineEKFInterpolator::canPredict() {
    if (stateCount <= (3 + polyDegree) * 3) {
        return false;
    }
    const Eigen::MatrixXd conv = ekf->getCovariance();
    Eigen::MatrixXd drivative = measurementJacobianHelper(ekf->getState(), polyDegree, ekf->time);

    double var = (drivative * conv * drivative.transpose())(0);

    return var < (totalVariance/updateCount) * 0.25;
}

double measurementFunctionHelper(const Eigen::VectorXd &state, double time, unsigned int polyDegree) {
    double value = state(0) * std::sin(state(2) * time);
    value += state(1) * std::cos(state(2) * time);

    for (int i = 0; i < polyDegree; i++) {
        value += state(i+3) * std::pow(time,i);
    }
    return value;
}

double PolySineEKFInterpolator::predict(double time) {
    return measurementFunctionHelper(ekf->getState(),time,polyDegree);
}


void PolySineEKFInterpolator::update(double measurement, double time) {
    ekf->time = time;
    ekf->predict();
    Eigen::VectorXd m = Eigen::VectorXd(1);
    m(0) = measurement;
    ekf->measure(m);
    updateCount++;
    stateCount++;

    const double delta = measurement - meanValue;
    meanValue += delta / updateCount;
    totalVariance +=  delta * (measurement - meanValue);
}

void PolySineEKFInterpolator::HelperEKF::predict() {
    if (hasPredicted) {
        P_ = P_ + Q_ * (time - lastPredictTime);
    }
    lastPredictTime = time;
    hasPredicted = true;
}

Eigen::VectorXd PolySineEKFInterpolator::HelperEKF::measurementFunction(const Eigen::VectorXd &state) {
    Eigen::VectorXd result(1);
    result(0) = measurementFunctionHelper(state, time, polyDegree);
    return result;
}


Eigen::VectorXd PolySineEKFInterpolator::HelperEKF::predictionFunction(const Eigen::VectorXd &state) {
    return state;
}

Eigen::MatrixXd PolySineEKFInterpolator::HelperEKF::predictionJacobian(const Eigen::VectorXd &state) {
    return Eigen::MatrixXd::Identity(polyDegree + 3, polyDegree + 3);
}


Eigen::MatrixXd PolySineEKFInterpolator::HelperEKF::measurementJacobian(const Eigen::VectorXd &state) {
    return measurementJacobianHelper(state,polyDegree,time);
}

