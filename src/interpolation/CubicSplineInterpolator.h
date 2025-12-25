#pragma once
#include "Interpolation/InterpolatorBase.h"
#include <Eigen/Dense>
#include "Algorithms/splines/CubicSpline.h"
#include <vector>
#include <memory>
#define  CUBIC_SPLINE_WINDOW_SIZE 30

class CubicSplineInterpolator : public InterpolatorBase {
    public:
        CubicSplineInterpolator(Eigen::VectorXd params);

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;

        static size_t getParamsCount();
        static Eigen::VectorXd getLowerBounds();
        static Eigen::VectorXd getUpperBounds();

    private:

        std::vector<double> times;
        std::vector<double> measurements;
        std::unique_ptr<CubicSpline> spline;
};

