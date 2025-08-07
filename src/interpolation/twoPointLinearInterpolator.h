#pragma once
#include "interpolation/interpolatorBase.h"

class TwoPointLinearInterpolator : public InterpolatorBase {
    public:
        TwoPointLinearInterpolator(double ts);

        InterpolatorBase* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;


    private:
        double timeA;
        double measurementA;

        double timeB;
        double measurementB;
        int updatedCount = 0;
};