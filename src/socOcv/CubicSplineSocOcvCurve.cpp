#include "socOcv/CubicSplineSocOcvCurve.h"

CubicSplineSocOcvCurve::CubicSplineSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    spline = new CubicSpline(data.soc, data.ocv);
}

double CubicSplineSocOcvCurve::getOcv(double soc) {
    return spline->predict(soc);
}

double CubicSplineSocOcvCurve::getOcvSocDerivative(double soc) {
    return spline->derivative(soc);
}