#pragma once
#include "interpolation/interpolatorBase.h"
#include "loading/processNasaCycles.h"
#include <iostream>

class InterpolatorEvaluator: public CycleHandler {
    public:
        InterpolatorEvaluator(InterpolatorBase* interpolator);

        double getCurrentMSE();
        double getVoltageMSE();
        double getTemperatureMSE();
        // onCharge, onDischarge, onImpedance
        void onImpedance(double Rct, double Re) override;
        void onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) override;
        void onDischarge(const Eigen::VectorXd &voltage,
                                const Eigen::VectorXd &current,
                                const Eigen::VectorXd &temperature,
                                const Eigen::VectorXd &time,
                                const double &capacity) override;
    
        void printResults();

    private:
        void stepThroughEvaluation(const Eigen::VectorXd &voltage,
                          const Eigen::VectorXd &current,
                          const Eigen::VectorXd &temperature,
                          const Eigen::VectorXd &time);

        void makePrediction(double ts, double v, double c, double t);

        InterpolatorBase* voltageInterpolator;
        InterpolatorBase* currentInterpolator;
        InterpolatorBase* temperatureInterpolator;

        double totalVoltageSE = 0;
        double totalCurrentSE = 0;
        double totalTemperatureSE = 0;
        
        double meanVoltage = 0;
        double meanCurrent = 0;
        double meanTemprature = 0;

        double totalVoltageVariance = 0;
        double totalCurrentVariance = 0;
        double totalTempratureVariance = 0;

        int predictionCount = 0;
};