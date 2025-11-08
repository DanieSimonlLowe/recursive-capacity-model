#include "SocOcv/PolyLsSocOcvCurve.h"

PolyLsSocOcvCurve::PolyLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    power = static_cast<int>(params(0));

    Eigen::Index n = data.soc.size();
    Eigen::MatrixXd V(n, power + 1);
    for (Eigen::Index i = 0; i < n; ++i) {
        double value = 1.0;
        for (int j = 0; j <= power; ++j) {
            V(i, j) = value;
            value *= data.soc(i);
        }
    }
    
    ls = new LeastSquares(V, data.ocv, std::pow(10, params(1)));
}

double PolyLsSocOcvCurve::getOcv(double soc) {
    Eigen::VectorXd input(power+1);

    double value = 1.0;
    for (int i = 0; i <= power; ++i) {
        input(i) = value;
        value *= soc;
    }

    return ls->predict(input);
}

double PolyLsSocOcvCurve::getOcvSocDerivative(double soc) {
    Eigen::VectorXd input(power+1);

    input(0) = 0;
    double value = 1.0;
    for (int i = 1; i <= power; ++i) {
        input(i) = value * i;
        value *= soc;
    }

    return ls->predict(input);
}

size_t PolyLsSocOcvCurve::getParamsCount() {
    return 2;
}

const Eigen::VectorXd PolyLsSocOcvCurve::getLowerBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd lower(2);
    lower[0] = 1.0;
    lower[1] = -25.0; // 10^-8
    
    return lower;
}

const Eigen::VectorXd PolyLsSocOcvCurve::getUpperBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd upper(2);
    upper[0] = 25.0;
    upper[1] = 6.0; // 10^4 / 10000
    
    return upper;
}