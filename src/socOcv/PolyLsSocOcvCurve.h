#pragma once
#include <Eigen/Dense>
#include <cstddef>
#include <cmath>
#include "socOcv/SocOcvCurveBase.h"
#include "algorithms/LeastSquares.h"

class PolyLsSocOcvCurve: public SocOcvCurveBase {
    public:
        PolyLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc) override;
        double getOcvSocDerivative(double soc) override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        LeastSquares *ls;
        int power;
};