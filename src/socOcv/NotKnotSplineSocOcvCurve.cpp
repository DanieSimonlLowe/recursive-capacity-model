#include "socOcv/NotKnotSplineSocOcvCurve.h"

NotKnotSplineSocOcvCurve::NotKnotSplineSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    spline = new NotKnotSpline(data.soc, data.ocv);
}

double NotKnotSplineSocOcvCurve::getOcv(double soc) {
    return spline->predict(soc);
}

double NotKnotSplineSocOcvCurve::getOcvSocDerivative(double soc) {
    return spline->derivative(soc);
}