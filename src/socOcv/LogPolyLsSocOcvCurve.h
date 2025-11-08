#pragma once
#include <Eigen/Dense>
#include <cstddef>
#include <cmath>
#include "SocOcv/SocOcvCurveBase.h"
#include "Algorithms/LeastSquares.h"

// https://arxiv.org/pdf/2306.16542
// Nernst model added paramters reduces to the model in the paper when power is 0.
class LogPolyLsSocOcvCurve: public SocOcvCurveBase {
    public:
        LogPolyLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc) override;
        double getOcvSocDerivative(double soc) override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        LeastSquares *ls;
        int power;
};