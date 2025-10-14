#pragma once
#include "algorithms/splines/PolynomialSpline.h"
#include "socOcv/SocOcvCurveBase.h"

class PolySplineOcvSoc: SocOcvCurveBase {
    public:
        PolySplineOcvSoc(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc);
        double getOcvSocDerivative(double soc);

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        PolynomialSpline *spline;
};
