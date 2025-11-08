#include "SocOcv/LogNegPolyLsSocOcvCurve.h"

LogNegPolyLsSocOcvCurve::LogNegPolyLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    power = static_cast<int>(params(0));
    negPower = static_cast<int>(params(2));

    Eigen::Index n = data.soc.size();
    Eigen::MatrixXd V(n, power + 4 + negPower);
    for (Eigen::Index i = 0; i < n; ++i) {
        double value = 1.0;
        double soc =  data.soc(i);

        for (int j = 0; j <= power; ++j) {
            V(i, j) = value;
            value *= soc;
        }
        
        if (soc < 1e-8) {
            soc = 1e-8;
        } 
        value = 1.0 / soc;
        for (int j = 0; j <= negPower; ++j) {
            V(i,j+power+1) = value;
            value /= soc;
        }

        if (soc > 1 - 1e-8) {
            soc = 1 - 1e-8;
        }

        V(i,power+negPower+2) = std::log(soc);
        V(i,power+negPower+3) = std::log(1-soc);
    }

    
    
    ls = new LeastSquares(V, data.ocv, std::pow(10, params(1)));
}

double LogNegPolyLsSocOcvCurve::getOcv(double soc) {
    Eigen::VectorXd input(power + 4 + negPower);

    double value = 1.0;
    for (int i = 0; i <= power; ++i) {
        input(i) = value;
        value *= soc;
    }

    if (soc <= 1e-8) {
        soc = 1e-8;
    }

    value = 1.0 / soc;
    for (int j = 0; j <= negPower; ++j) {
        input(j+power+1) = value;
        value /= soc;
    }

    if (soc > 1 - 1e-8) {
        soc = 1 - 1e-8;
    }

    input(power+negPower+2) = std::log(soc);
    input(power+negPower+3) = std::log(1-soc);

    return ls->predict(input);
}

double LogNegPolyLsSocOcvCurve::getOcvSocDerivative(double soc) {
    Eigen::VectorXd input(power + 2 + negPower);

    input(0) = 0;
    double value = 1.0;
    for (int i = 1; i <= power; ++i) {
        input(i) = value * i;
        value *= soc;
    }
    if (soc <= 1e-8) {
        soc = 1e-8;
    }
    value = 1.0 / (soc*soc);
    for (int j = 1; j <= negPower; ++j) {
        input(j+power+1) = value;
        value /= - j * soc;
    }

    if (soc > 1 - 1e-8) {
        soc = 1 - 1e-8;
    }

    input(power+negPower+2) = 1/soc;
    input(power+negPower+3) = -1/(1-soc);

    return ls->predict(input);
}

size_t LogNegPolyLsSocOcvCurve::getParamsCount() {
    return 3;
}

const Eigen::VectorXd LogNegPolyLsSocOcvCurve::getLowerBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd lower(3);
    lower[0] = 0.0;
    lower[2] = 0.0;
    lower[1] = -100.0; 
    
    return lower;
}

const Eigen::VectorXd LogNegPolyLsSocOcvCurve::getUpperBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd upper(3);
    upper[0] = 50.0;
    upper[2] = 25.0; // neg power
    upper[1] = 6.0; 
    
    return upper;
}