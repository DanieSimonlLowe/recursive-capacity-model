#pragma once
#include <vector>


class ECMStateEstimator {
    public:
        ~ECMStateEstimator() = default;

        virtual double getOhmicResistance() = 0;
        
        virtual const std::vector<double> getBranchResistances() = 0;

        virtual const std::vector<double>  getBranchCapacities() = 0;

        virtual bool canCalculateState() const = 0;
    
        virtual void update(const std::vector<double> current, 
                    const std::vector<double> voltage);

        virtual void setDeltaTime(double deltaTime);

        //static size_t getParamsCount() { return 0; };
        //static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        //static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};
