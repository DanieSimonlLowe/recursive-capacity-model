#pragma once
#include <Eigen/Dense>
#include <cstddef>
#include <cmath>
#include "SocToOcv/SocToOcvBase.h"
#include "Algorithms/LeastSquares.h"

// https://arxiv.org/pdf/2306.16542
// Nernst model added paramters reduces to the model in the paper when power is 0.
class LogPolyLsSocToOcv: public SocToOcvBase {
    public:
        LogPolyLsSocToOcv(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc) override;
        double getOcvToSocDerivative(double soc) override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        LeastSquares *ls;
        int power;
};


