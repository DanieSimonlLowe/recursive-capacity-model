#pragma once
#include <Eigen/Dense>
#include <cstddef>
#include <cmath>
#include "SocToOcv/SocToOcvBase.h"
#include "Algorithms/LeastSquares.h"

class NegPolyLsSocToOcv: public SocToOcvBase {
    public:
        NegPolyLsSocToOcv(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc) override;
        double getOcvToSocDerivative(double soc) override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        LeastSquares *ls;
        int power;
        int negPower;
};


