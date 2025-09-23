#include "socOcv/ExpLsSocOcvCurve.h"

ExpLsSocOcvCurve::ExpLsSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    power = static_cast<int>(params(0));
    negPower = static_cast<int>(params(2));
    Eigen::Index n = data.soc.size();
    Eigen::MatrixXd V(n, power + 1 + negPower);
    for (Eigen::Index i = 0; i < n; ++i) {
        double soc =  data.soc(i);
        V(i,0) = 1;
        double value = soc;
        for (int j = 1; j < power; ++j) {
            V(i, j) = std::exp(value);
            
            value *= soc;
        }

        value = soc;
        for (int j = 1; j < negPower; ++j) {
            V(i, j+power) = std::exp(-value);
            value *= soc;
        }
    }
    
    ls = new LeastSquares(V, data.ocv, std::pow(10, params(1)));
}

double ExpLsSocOcvCurve::getOcv(double soc) {
    Eigen::VectorXd input(power + 1 + negPower);

    input(0) = 1;

    double value = soc;
    for (int i = 1; i <= power; ++i) {
        input(i) = std::exp(value);
        value *= soc;
    }

    value = soc;
    for (int i = 1; i <= negPower; ++i) {
        input(i+power) = std::exp(-value);
        value *= soc;
    }
    return ls->predict(input);
}

double ExpLsSocOcvCurve::getOcvSocDerivative(double soc) {
    Eigen::VectorXd input(power + 1 + negPower);
    // TODO do this If I use it, it dose not make sense to reimplent this every time.
    // input(0) = 0;
    // double value = 1.0;
    // for (int i = 1; i <= power; ++i) {
    //     input(i) = value * i;
    //     value *= soc;
    // }

    // value = soc;
    // for (int i = 0; i <= power; ++i) {
    //     input(i+power+1) = std::exp(value);
    // }

    return ls->predict(input);
}

size_t ExpLsSocOcvCurve::getParamsCount() {
    return 3;
}

const Eigen::VectorXd ExpLsSocOcvCurve::getLowerBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd lower(3);
    lower[0] = 0.0;
    lower[2] = 0.0;
    lower[1] = -80.0; 
    
    return lower;
}

const Eigen::VectorXd ExpLsSocOcvCurve::getUpperBounds() {
    // 0: power,
    // 1: regularization constant, (do log scale)
    Eigen::VectorXd upper(3);
    upper[0] = 50.0;
    upper[2] = 25.0; // neg power
    upper[1] = 6.0; 
    
    return upper;
}