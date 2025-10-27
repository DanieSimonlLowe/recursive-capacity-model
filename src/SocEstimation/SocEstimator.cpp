#include "SocEstimation/SocEstimator.h"

void SocEstimator::setOhmicResistance(const double value) {
    ohmicResistance = value;
}

void SocEstimator::setCapacity(const double value) {
    capacity = value;
}

void SocEstimator::setBranchResistances(const std::vector<double> value) {
    branchResistances = value;
}

void SocEstimator::setBranchCapacities(const std::vector<double> value) {
    branchCapacities = value;
}