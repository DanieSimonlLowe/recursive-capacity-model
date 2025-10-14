#pragma once
#include "interpolation/interpolatorBase.h"
#include <Eigen/Dense>
#include "algorithms/splines/AkimaSpline.h"
#include <vector>
#include <memory>

class AkimaSplineInterpolator : public InterpolatorBase {
    public:
        AkimaSplineInterpolator(double ts, unsigned int windowSize);

        AkimaSplineInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;

    private:
        unsigned int windowSize;

        std::vector<double> times;
        std::vector<double> measurements;
        std::unique_ptr<AkimaSpline> spline;
};

