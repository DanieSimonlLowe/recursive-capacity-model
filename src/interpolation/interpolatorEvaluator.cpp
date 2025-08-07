#include "interpolation/interpolatorEvaluator.h"

void InterpolatorEvaluator::onImpedance(double Rct, double Re) {
    // Should do nothing
}

InterpolatorEvaluator::InterpolatorEvaluator(InterpolatorBase* interpolator) {
    voltageInterpolator = interpolator->clone();
    currentInterpolator = interpolator->clone();
    temperatureInterpolator = interpolator;
}

void InterpolatorEvaluator::stepThroughEvaluation(const Eigen::VectorXd &voltage,
                          const Eigen::VectorXd &current,
                          const Eigen::VectorXd &temperature,
                          const Eigen::VectorXd &time) {
    const Eigen::Index n = voltage.size(); 
    voltageInterpolator->reset();
    currentInterpolator->reset();
    temperatureInterpolator->reset();
    bool hasOld = false;
    double oldV, oldC, oldT, oldTs;
    for (Eigen::Index i = 0; i < n; ++i) {
        double v = voltage[i];
        double c = current[i];
        double t = temperature[i];
        double ts = time[i];

        if (!voltageInterpolator->canPredict() || i % 2 == 1) {
            voltageInterpolator->update(v,ts);
            currentInterpolator->update(c,ts);
            temperatureInterpolator->update(t,ts);

            makePrediction(oldTs,oldV,oldC,oldT);
        } else {
            hasOld = true;
            oldV = v;
            oldT = t;
            oldTs = ts;
            oldC = c;
        }
    }
}

void InterpolatorEvaluator::makePrediction(double ts, double v, double c, double t) {
    double pv = voltageInterpolator->predict(ts);
    double pc = currentInterpolator->predict(ts);
    double pt = temperatureInterpolator->predict(ts);

    totalVoltageSE += (pv - v) * (pv - v);
    totalCurrentSE += (pc - c) * (pc - c);
    totalTemperatureSE += (pt - t) * (pt - t);
    predictionCount++;


    const double deltaVoltage = v - meanVoltage;
    meanVoltage += deltaVoltage / predictionCount;
    totalVoltageVariance += deltaVoltage * (v - meanVoltage);

    const double deltaCurrent = c - meanCurrent;
    meanCurrent += deltaCurrent / predictionCount;
    totalCurrentVariance += deltaCurrent * (c - meanCurrent);

    double deltaTemperature = t - meanTemprature;
    meanTemprature += deltaTemperature / predictionCount;
    totalTempratureVariance += deltaTemperature * (t - meanTemprature);
}

void InterpolatorEvaluator::onCharge(const Eigen::VectorXd &voltage,
                          const Eigen::VectorXd &current,
                          const Eigen::VectorXd &temperature,
                          const Eigen::VectorXd &time) {
    this->stepThroughEvaluation(voltage, current, temperature, time);
    }

void InterpolatorEvaluator::onDischarge(const Eigen::VectorXd &voltage,
                          const Eigen::VectorXd &current,
                          const Eigen::VectorXd &temperature,
                          const Eigen::VectorXd &time,
                          const double &capacity) {
    this->stepThroughEvaluation(voltage, current, temperature, time);
}

double InterpolatorEvaluator::getCurrentMSE() {
    return totalVoltageSE / predictionCount;
}

double InterpolatorEvaluator::getVoltageMSE() {
    return totalCurrentSE / predictionCount;
}

double InterpolatorEvaluator::getTemperatureMSE() {
    return totalTemperatureSE / predictionCount;
}

void InterpolatorEvaluator::printResults() {
    std::cout << "evaluate " << typeid(voltageInterpolator).name() << " \n";
    processNasaCycles("/mnt/c/Users/Danie/Desktop/project/data/B0007.mat","B0007",*this);

    std::cout << "Current \tMSE " << getCurrentMSE() 
            << "\t R^2 " << (getCurrentMSE() / (totalCurrentVariance / (predictionCount-1))) 
            << " \tVar: " << (totalCurrentVariance / (predictionCount-1)) << " \n";
    std::cout << "Voltage \tMSE " << getVoltageMSE() 
            << "\t R^2 " << (getVoltageMSE() / (totalVoltageVariance / (predictionCount-1))) 
            << " \tVar: " << (totalVoltageVariance / (predictionCount-1)) << " \n";
    std::cout << "Temperature \tMSE " << getTemperatureMSE() 
            << "\t R^2 " << (getTemperatureMSE() / (totalTempratureVariance / (predictionCount-1)))
            << " \tVar: " << (totalTempratureVariance / (predictionCount-1)) << " \n";
}