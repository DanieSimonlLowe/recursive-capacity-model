#pragma once
#include <vector>
#include "SocToOcv/SocToOcvBase.h"

class SocEstimator {
    public:
        ~SocEstimator() = default;

        virtual void setOhmicResistance(const double value);
        
        virtual void setBranchResistances(const std::vector<double> value);

        virtual void setBranchCapacities(const std::vector<double> value);

        virtual void setCapacity(const double value);
    

        virtual double predictVoltage(const double current, const double deltaTime) = 0;

        virtual double getSoc() = 0;

        virtual void measure(const double current, const double voltage) = 0;

        virtual void setOcv(const double ocv) = 0;


        protected:
            double ohmicResistance;
            std::vector<double> branchResistances;
            std::vector<double> branchCapacities;
            double capacity;

        //static size_t getParamsCount() { return 0; };
        //static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        //static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};

