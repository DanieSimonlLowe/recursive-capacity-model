#include "SocOcv/NegPolyLsSocOcvCurve.h"

NegPolyLsSocOcvCurve::NegPolyLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    power = static_cast<int>(params(0));
    negPower = static_cast<int>(params(2));

    Eigen::Index n = data.soc.size();
    Eigen::MatrixXd V(n, power + 2 + negPower);
    for (Eigen::Index i = 0; i < n; ++i) {
        double value = 1.0;
        double soc =  data.soc(i);

        for (int j = 0; j <= power; ++j) {
            V(i, j) = value;
            value *= soc;
        }
        
        if (soc <= 1e-8) {
            soc = 1e-8;
        }
        value = 1.0 / soc;
        for (int j = 0; j <= negPower; ++j) {
            V(i,j+power+1) = value;
            value /= soc;
        }
    }
    
    ls = new LeastSquares(V, data.ocv, std::pow(10, params(1)));
}

double NegPolyLsSocOcvCurve::getOcv(double soc) {
    Eigen::VectorXd input(power + 2 + negPower);

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

    return ls->predict(input);
}

double NegPolyLsSocOcvCurve::getOcvSocDerivative(double soc) {
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

    return ls->predict(input);
}

size_t NegPolyLsSocOcvCurve::getParamsCount() {
    return 3;
}

const Eigen::VectorXd NegPolyLsSocOcvCurve::getLowerBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd lower(3);
    lower[0] = 0.0;
    lower[2] = 0.0;
    lower[1] = -80.0; 
    
    return lower;
}

const Eigen::VectorXd NegPolyLsSocOcvCurve::getUpperBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd upper(3);
    upper[0] = 50.0;
    upper[2] = 25.0; // neg power
    upper[1] = 6.0; 
    
    return upper;
}