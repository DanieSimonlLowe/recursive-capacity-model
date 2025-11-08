#include "SocOcv/LogExpOcvSocCurve.h"

LogExpOcvSocCurve::LogExpOcvSocCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    double learningRate = std::pow(10,params(0)); 
    const double learningRateDecay = 1 - 1/params(1); // value can't be grater then 1.
    const double minChange = 1e-12; // not a input as basicaly how long untill stops.
    const double maxValue = 1e6;

    int size = data.soc.size();
    Eigen::VectorXd socs(size);
    Eigen::VectorXd change(6);
    Eigen::VectorXd values(6);
    Eigen::MatrixXd derivative(6,size);
    for (int i = 0; i<size; ++i) {
        derivative(0,i) = 1; // because reMains the same.
        derivative(2,i) = data.soc(i);

        if (data.soc(i) <= 1e-8) {
            socs(i) = 1e-8;
        } else if (data.soc(i) >= 1-1e-8) {
            socs(i) = 1-1e-8;
        } else {
            socs(i) = data.soc(i);
        }
    }

    Eigen::VectorXd func(size);
    //a,b,c,d,m,n
    values << (data.ocv.mean()-0.5),1,1,1,1,1;
    int loop = 0;
    do {
        for (int i = 0; i<size; ++i) {
            const double soc = socs(i);
            
            double logsoc = std::log(soc);
            const double expfunc = std::exp(values(5)*(soc-1));
            const double logpowfunc = std::pow(-logsoc,values(4));

            if (logsoc >= -1e-8) {
                logsoc = -1e-8;
            }

            func(i) = values(0) + values(1) * logpowfunc 
                        + values(2) * soc + values(3) * expfunc;
            
            //derivative(0,i) = 1;
            derivative(1,i) = logpowfunc;
            //derivative(2,i) = soc;
            derivative(3,i) = expfunc;
            derivative(4,i) = values(1) * std::log(-logsoc)*logpowfunc;
            derivative(5,i) = values(3) * (soc - 1) * expfunc;
        }
        change = learningRate * derivative * (data.ocv-func);
        
        for (int i = 0; i < 6; i++) {
            if (!std::isfinite(change(i))) {
                change(i) = 0;
            }
        }
        
        values += change;

        for (int i = 0; i < 4; i++) { // to prevent value explsion that can prevent hyper paramter tuning.
            if (values(i) < -maxValue) {
                values(i) = -maxValue; // m > 0
            } else if (values(i) > maxValue) {
                values(i) = maxValue;
            }
        }

        if (values(4) < 1e-8) {
            values(4) = 1e-8; // m > 0
        } else if (values(4) > maxValue) {
            values(4) = maxValue;
        }
        if (values(5) < 1e-8) {
            values(5) = 1e-8; // n > 0
        }  if (values(4) > maxValue) {
            values(4) = maxValue;
        }

        
        learningRate *= learningRateDecay;
        loop++;
        //std::cout << change.squaredNorm() << std::endl;
    } while (change.squaredNorm() > minChange && loop < 1000);

    
    a = values(0);
    b = values(1);
    c = values(2);
    d = values(3);
    m = values(4);
    n = values(5);

    // std::cout << "a: " << a << std::endl;
    // std::cout << "b: " << b << std::endl;
    // std::cout << "c: " << c << std::endl;
    // std::cout << "d: " << d << std::endl;
    // std::cout << "m: " << m << std::endl;
    // std::cout << "n: " << n << std::endl;
}

double LogExpOcvSocCurve::getOcv(double soc) {
    if (soc < 1e-8) { 
        soc = 1e-8;
    } else if (soc > 1-1e8) {
        soc = 1-1e-8;
    }
    return a + b * std::pow(-std::log(soc),m) + c * soc + d * std::exp(n*(soc-1));
}

double LogExpOcvSocCurve::getOcvSocDerivative(double soc) {
    if (soc < 1e-8) { 
        soc = 1e-8;
    } else if (soc > 1-1e8) {
        soc = 1-1e-8;
    }
    double logsoc = std::log(soc);
    return b * m * std::pow(-logsoc,m) / (soc * logsoc) + c + d * n * std::exp(n * (soc - 1));
}

size_t LogExpOcvSocCurve::getParamsCount() {
    return 2;
}

const Eigen::VectorXd LogExpOcvSocCurve::getLowerBounds() {
    Eigen::VectorXd lower(2);
    lower[0] = -6; // 1e-3
    lower[1] = 10; // 0.75
    
    return lower;
}

const Eigen::VectorXd LogExpOcvSocCurve::getUpperBounds() {
    Eigen::VectorXd upper(2);
    upper[0] = 0; // 1
    upper[1] = 1000; // 0.99
    
    return upper;
}