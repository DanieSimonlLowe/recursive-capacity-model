#pragma once
#include "socOcv/SocOcvCurveBase.h"
#include "algorithms/cubicSpline.h"

class CubicSplineSocOcvCurve: SocOcvCurveBase {
    public:
        CubicSplineSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc) override;
        double getOcvSocDerivative(double soc) override;
    
    private:
        CubicSpline *spline;
};