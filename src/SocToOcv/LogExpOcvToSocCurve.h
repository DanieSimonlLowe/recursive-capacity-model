#pragma once
#include <Eigen/Dense>
#include <cstddef>
#include <cmath>

#include "SocToOcv/SocToOcvBase.h"
#include "Algorithms/LeastSquares.h"


// algorthim based on 
// https://www.mdpi.com/1996-1073/9/11/900
class LogExpOcvToSocCurve: public SocToOcvBase {
    public:
        LogExpOcvToSocCurve(const OcvSocData &data, const Eigen::VectorXd& params);

        double getOcv(double soc) override;
        double getOcvToSocDerivative(double soc) override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        double a,b,c,d,m,n;
};


