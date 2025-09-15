#pragma once
#include "ECMStateEstimator.h"
#include "algorithms/recursiveLeastSquares.h"
#include <cmath>
#include <Eigen/Dense>

// RLS2ECMStateEstimator: Estimates battery states using Recursive Least Squares (RLS) 
// with a second-order Equivalent Circuit Model (2ECM).
// https://www.mdpi.com/1996-1073/12/12/2242
class RLS2ECMStateEstimator : public  ECMStateEstimator {
    public:
        RLS2ECMStateEstimator(RecursiveLeastSquares* rls, double deltaTime) override;

        void update(const std::vector<double> current, 
                    const std::vector<double> voltage) override;

        double getOhmicResistance() override;
        
        const std::vector<double> getBranchResistances() override;

        const std::vector<double>  getBranchCapacities() override;
    
        bool canCalculateState() const override;
    
    private:
        bool stateCalculated;
        void calculateState();
        
        RecursiveLeastSquares* rls;
        double deltaTime;

        double R0;
        double R1;
        double R2;
        double C1;
        double C2;
};

