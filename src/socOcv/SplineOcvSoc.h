#pragma once
#include <cstddef>
#include "socOcv/SocOcvCurveBase.h"

template<typename T>
class SplineOcvSoc: SocOcvCurveBase {
    public:
        SplineOcvSoc(const OcvSocData &data, const Eigen::VectorXd& params = Eigen::VectorXd());

        double getOcv(double soc);
        double getOcvSocDerivative(double soc);

        static size_t getParamsCount() { return 0; };
        static const Eigen::VectorXd getLowerBounds() { return Eigen::VectorXd(); };
        static const Eigen::VectorXd getUpperBounds() { return Eigen::VectorXd(); };
    
    private:
        T *spline;
};

#include "socOcv/SplineOcvSoc.tpp"