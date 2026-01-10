#include "SocToOcv/LogPolyLsSocToOcv.h"

LogPolyLsSocToOcv::LogPolyLsSocToOcv(const OcvSocData &data, const Eigen::VectorXd& params) {
    power = static_cast<int>(params(0));

    Eigen::Index n = data.soc.size();
    Eigen::MatrixXd V(n, power + 3);
    for (Eigen::Index i = 0; i < n; ++i) {
        double value = 1.0;
        for (int j = 0; j <= power; ++j) {
            V(i, j) = value;
            value *= data.soc(i);
        }
        
        double soc = data.soc(i);
        if (soc < 1e-8) {
            soc = 1e-8;
        } else if (soc > 1 - 1e-8) {
            soc = 1 - 1e-8;
        }
        V(i,power+1) = std::log(soc);
        V(i,power+2) = std::log(1-soc);
    }
    
    ls = new LeastSquares(V, data.ocv, std::pow(10, params(1)));
}

double LogPolyLsSocToOcv::getOcv(double soc) {
    Eigen::VectorXd input(power+3);

    double value = 1.0;
    for (int i = 0; i <= power; ++i) {
        input(i) = value;
        value *= soc;
    }
    
    if (soc < 1e-8) {
        soc = 1e-8;
    } else if (soc > 1 - 1e-8) {
        soc = 1 - 1e-8;
    }
    input(power+1) = std::log(soc);
    input(power+2) = std::log(1-soc);

    return ls->predict(input);
}

double LogPolyLsSocToOcv::getOcvToSocDerivative(double soc) {
    Eigen::VectorXd input(power+3);

    input(0) = 0;
    double value = 1.0;
    for (int i = 1; i <= power; ++i) {
        input(i) = value * i;
        value *= soc;
    }
    if (soc < 1e-8) {
        soc = 1e-8;
    } else if (soc > 1 - 1e-8) {
        soc = 1 - 1e-8;
    }
    input(power+1) = 1/soc;
    input(power+2) = -1/(1-soc);

    return ls->predict(input);
}

size_t LogPolyLsSocToOcv::getParamsCount() {
    return 2;
}

const Eigen::VectorXd LogPolyLsSocToOcv::getLowerBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd lower(2);
    lower[0] = 0.0;
    lower[1] = -25.0; // 10^-8
    
    return lower;
}

const Eigen::VectorXd LogPolyLsSocToOcv::getUpperBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd upper(2);
    upper[0] = 35.0;
    upper[1] = 6.0; // 10^4 / 10000
    
    return upper;
}


