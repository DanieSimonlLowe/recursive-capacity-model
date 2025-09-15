#pragma once
#include "interpolation/interpolatorBase.h"


class HermiteSplineInterpolator : public InterpolatorBase {
    public:
        HermiteSplineInterpolator(double ts);

        HermiteSplineInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;


    private:
        double t0, y0;
        double t1, y1;
        double m0, m1;
        unsigned int counter;
        
};