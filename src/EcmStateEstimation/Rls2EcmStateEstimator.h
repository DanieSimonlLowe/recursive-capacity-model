#pragma once
#include <cmath>
#include <Eigen/Dense>
#include "EcmStateEstimation/ECMStateEstimator.h"




// Rls2EcmStateEstimator: Estimates battery states using Recursive Least Squares (RLS) 
// with a second-order Equivalent Circuit Model (2ECM).
// https://www.mdpi.com/1996-1073/12/12/2242
template<typename RLS>
class Rls2EcmStateEstimator : public  ECMStateEstimator {
    public:
        Rls2EcmStateEstimator(const Eigen::VectorXd& params);

        void update(const Eigen::VectorXd& current, 
                    const Eigen::VectorXd& voltage) override;

        void setDeltaTime(double deltaTime) override;
        
        double getOhmicResistance() override;
        
        const std::vector<double> getBranchResistances() override;

        const std::vector<double>  getBranchCapacities() override;
    
        bool canCalculateState() const override;
        
        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
        static const int getDimension();
    
    private:
        bool stateCalculated;
        void calculateState();
        
        RLS* rls;
        double deltaTime;

        double R0;
        double R1;
        double R2;
        double C1;
        double C2;
};

#include "EcmStateEstimation/Rls2EcmStateEstimator.tpp"