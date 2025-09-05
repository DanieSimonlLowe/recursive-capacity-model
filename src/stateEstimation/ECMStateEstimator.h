#pragma once
#include <vector>

class ECMStateEstimator {
    public:
        virtual ~ECMStateEstimator() = default;

        virtual double getOhmicResistance() = 0;
        
        virtual const std::vector<double> getBranchResistances() = 0;

        virtual const std::vector<double>  getBranchCapacities() = 0;

        virtual bool canCalculateState() const = 0;
    
};
