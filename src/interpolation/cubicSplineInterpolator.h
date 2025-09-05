#pragma once
#include "interpolation/interpolatorBase.h"
#include <Eigen/Dense>
#include "algorithms/cubicSpline.h"
#include <vector>
#include <memory>

class CubicSplineInterpolator : public InterpolatorBase {
    public:
        CubicSplineInterpolator(double ts, unsigned int windowSize);

        CubicSplineInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;

    private:
        unsigned int windowSize;

        std::vector<double> times;
        std::vector<double> measurements;
        std::unique_ptr<CubicSpline> spline;
};

