#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <matio.h>
#include <Eigen/Dense>

// Abstract handler interface for processing cycles
class CycleHandler {
    public:
        virtual ~CycleHandler() = default;
        virtual void onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) = 0;

        virtual void onDischarge(const Eigen::VectorXd &voltage,
                                const Eigen::VectorXd &current,
                                const Eigen::VectorXd &temperature,
                                const Eigen::VectorXd &time,
                                const double &capacity) = 0;

        // Rct (Charge transfer resistance):
        // Re (Electrolyte resistance / ohmic resistance)
        virtual void onImpedance(double Rct, double Re) = 0;

};

void processNasaCycles(const std::string &matFile, const std::string &varbleName, CycleHandler &handler);
