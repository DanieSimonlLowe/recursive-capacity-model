#include "socOcv/PolySplineOcvSoc.h"

PolySplineOcvSoc::PolySplineOcvSoc(const OcvSocData &data, const Eigen::VectorXd& params) {
    int power = params(0);
    if (power % 2 == 0) {
        power++;
    }
    spline = new PolynomialSpline(data.soc, data.ocv,power);
}


double PolySplineOcvSoc::getOcv(double soc) {
    return spline->predict(soc);
}


double PolySplineOcvSoc::getOcvSocDerivative(double soc) {
    return spline->derivative(soc,1);
}

size_t PolySplineOcvSoc::getParamsCount() {
    return 1;
}

const Eigen::VectorXd PolySplineOcvSoc::getLowerBounds() { 
    Eigen::VectorXd lower(1);
    lower[0] = 3.0;
    
    return lower;
};
const Eigen::VectorXd PolySplineOcvSoc::getUpperBounds() { 
    Eigen::VectorXd upper(1);
    upper[0] = 15.0;
    
    return upper;
};