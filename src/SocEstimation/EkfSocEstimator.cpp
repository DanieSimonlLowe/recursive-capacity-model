#include "SocEstimation/EkfSocEstimator.h"

EkfSocEstimator::EkfSocEstimator(const int dimension, SocToOcvBase *SocToOcv,
                                 OcvToSocBase *OcvToSoc, const Eigen::VectorXd &params)
    : dimension(dimension), SocToOcv(SocToOcv), OcvToSoc(OcvToSoc) {

    const int n = dimension + 1; // State size: SOC + dimension RC voltages
    Eigen::VectorXd state = Eigen::VectorXd::Zero(n);

    const double initialSocVariance = std::pow(10, params(0));
    const double initialValtVariance = std::pow(10, params(1));

    Eigen::MatrixXd covariance = initialValtVariance * Eigen::MatrixXd::Identity(n, n);
    covariance(0, 0) = initialSocVariance;

    const double processSocNoise = std::pow(10, params(2));
    const double processValtNoise = std::pow(10, params(3));
    Eigen::MatrixXd processNoise = processValtNoise * Eigen::MatrixXd::Identity(n, n);
    processNoise(0, 0) = processSocNoise;

    Eigen::MatrixXd measurementNoise(1, 1);
    measurementNoise(0, 0) = std::pow(10, params(4));

    ekf = new EkfSocEstimator::HelperEKF(processNoise, measurementNoise, state, covariance,
                                         dimension, SocToOcv, OcvToSoc, this);
}

size_t EkfSocEstimator::getParamsCount() { return 5; }

const Eigen::VectorXd EkfSocEstimator::getLowerBounds() {
    Eigen::VectorXd vals(5);
    vals(0) = -5; // initial Soc Variance
    vals(1) = -5; // initial Valt Variance
    vals(2) = -8; // process Soc Noise
    vals(3) = -8; // process Valt Noise
    vals(4) = -5; // measurement Noise
    return vals;
}

const Eigen::VectorXd EkfSocEstimator::getUpperBounds() {
    Eigen::VectorXd vals(5);
    vals(0) = 2; // initial Soc Variance
    vals(1) = 2; // initial Valt Variance
    vals(2) = 1; // process Soc Noise
    vals(3) = 1; // process Valt Noise
    vals(4) = 1; // measurement Noise
    return vals;
}

void EkfSocEstimator::measure(const double current, const double voltage) {
    double reactanceVolt = voltage - current * ohmicResistance;

    Eigen::VectorXd measurement(1);
    measurement(0) = reactanceVolt;

    ekf->measure(measurement);
}

double EkfSocEstimator::predictVoltage(const double current, const double deltaTime) {
    ekf->setUpdateState(deltaTime, current);
    ekf->predict();

    Eigen::VectorXd state = ekf->getState();
    double predVolt = SocToOcv->getOcv(state(0)) - current * ohmicResistance;
    for (int i = 1; i <= dimension; i++) {
        predVolt -= state(i);
    }
    return predVolt;
}

void EkfSocEstimator::setOcv(const double ocv) {
    Eigen::VectorXd state = ekf->getState();
    state(0) = OcvToSoc->getSoc(ocv);
    ekf->setState(state);
}

double EkfSocEstimator::getSoc() { return ekf->getState()(0); }

EkfSocEstimator::HelperEKF::HelperEKF(const Eigen::MatrixXd &processNoise,
                                      const Eigen::MatrixXd &measurementNoise,
                                      const Eigen::VectorXd &initialState,
                                      const Eigen::MatrixXd &initialCovariance, const int dimension,
                                      SocToOcvBase *SocToOcv, OcvToSocBase *OcvToSoc,
                                      EkfSocEstimator *parent)
    : ExtendedKalmanFilter(processNoise, measurementNoise, initialState, initialCovariance),
      dimension(dimension), SocToOcv(SocToOcv), OcvToSoc(OcvToSoc), parent(parent) {
    // Fixed: measurementVector size should be dimension + 1 (state size)
    measurementVector = -1 * Eigen::VectorXd::Ones(dimension + 1);
    measurementVector(0) = 0;
}

void EkfSocEstimator::HelperEKF::setUpdateState(const double dt, const double c) {
    deltaTime = dt;
    current = c;
}

Eigen::VectorXd EkfSocEstimator::HelperEKF::measurementFunction(const Eigen::VectorXd &state) {
    Eigen::VectorXd out(1);
    out(0) = measurementVector.dot(state) + SocToOcv->getOcv(state(0));
    return out;
}

Eigen::MatrixXd EkfSocEstimator::HelperEKF::measurementJacobian(const Eigen::VectorXd &state) {
    Eigen::MatrixXd out = -1 * Eigen::MatrixXd::Ones(1, dimension + 1);
    out(0, 0) = SocToOcv->getOcvToSocDerivative(state(0));
    return out;
}

Eigen::VectorXd EkfSocEstimator::HelperEKF::predictionFunction(const Eigen::VectorXd &state) {
    Eigen::VectorXd out(dimension + 1);
    out(0) = state(0) - 1 / parent->capacity * current * deltaTime;
    for (int i = 0; i < dimension; i++) {
        const double e =
            std::exp(-(deltaTime) / (parent->branchResistances[i] * parent->branchCapacities[i]));
        out(i + 1) = state(i + 1) * e + parent->branchResistances[i] * (1 - e) * current;
    }
    return out;
}

// TODO: double check this Jacoban I think it may be wrong.
Eigen::MatrixXd EkfSocEstimator::HelperEKF::predictionJacobian(const Eigen::VectorXd &state) {
    Eigen::MatrixXd out = Eigen::MatrixXd::Zero(dimension + 1, dimension + 1);
    out(0, 0) = 1;
    for (int i = 0; i < dimension; i++) {
        const double e =
            std::exp(-(deltaTime) / (parent->branchResistances[i] * parent->branchCapacities[i]));
        out(i + 1, i + 1) = e;
    }
    return out;
}

void EkfSocEstimator::HelperEKF::clampState() {
    if (x_(0) > 1.0) {
        x_(0) = 1.0;
    } else if (x_(0) < 0.0) {
        x_(0) = 0.0;
    }
}

