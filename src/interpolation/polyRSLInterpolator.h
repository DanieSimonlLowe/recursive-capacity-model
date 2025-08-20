#pragma once
#include "interpolation/interpolatorBase.h"
#include "algorithms/recursiveLeastSquares.h"
#include <cmath>

class PolyRSLInterpolator : public InterpolatorBase {
    public:
        PolyRSLInterpolator(double ts, unsigned int polyDegree, double initialVariance, 
            double forgettingFactor, double resetVarianceMult);

        PolyRSLInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;
        void setParams(Eigen::VectorXd& params) override;
        size_t getParamsCount() override;
        Eigen::VectorXd getLowerBounds() override;
        Eigen::VectorXd getUpperBounds() override;


    private:
        unsigned int polyDegree;
        unsigned int updateCount;
        RecursiveLeastSquares* rsl;
        double resetVarianceMult;

        Eigen::VectorXd getVector(double time);
        
};