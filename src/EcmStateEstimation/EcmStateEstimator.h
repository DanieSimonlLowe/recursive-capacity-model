#pragma once
#include <Eigen/Dense>


class ECMStateEstimator {
    public:
        ~ECMStateEstimator() = default;

        virtual double getOhmicResistance() = 0;
        
        virtual const std::vector<double> getBranchResistances() = 0;

        virtual const std::vector<double>  getBranchCapacities() = 0;

        virtual bool canCalculateState() const = 0;
    
        virtual void update(const Eigen::VectorXd& current, 
                    const Eigen::VectorXd& voltage) = 0;

        virtual void setDeltaTime(double deltaTime) = 0;


        //static size_t getParamsCount() { return 0; };
        //static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        //static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};
