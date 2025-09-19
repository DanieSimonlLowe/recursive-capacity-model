#pragma once
#include <Eigen/Dense>
#include "loading/readOcvSocCSV.h"


class SocOcvCurveBase {
    public:
        ~SocOcvCurveBase() = default;

        virtual double getOcv(double soc) = 0;
        virtual double getOcvSocDerivative(double soc) = 0;
        
        virtual size_t getParamsCount() { return 0; };
        virtual const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        virtual const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
};