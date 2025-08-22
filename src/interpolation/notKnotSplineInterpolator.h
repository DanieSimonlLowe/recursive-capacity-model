#pragma once
#include "interpolation/interpolatorBase.h"
#include <Eigen/Dense>
#include "algorithms/notKnotSpline.h"
#include <vector>        

class NotKnotSplineInterpolator : public InterpolatorBase {
    public:
        NotKnotSplineInterpolator(double ts, unsigned int windowSize);

        NotKnotSplineInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;

    private:
        unsigned int windowSize;

        std::vector<double> times;
        std::vector<double> measurements;
};

