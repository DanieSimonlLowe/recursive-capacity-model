#pragma once
#include "interpolation/interpolatorBase.h"
#include "algorithms/cubicSpline.h"
#include <vector> 
#define  CUBIC_SPLINE_WINDOW_SIZE 50      

class FastCubicSplineInterpolator : public InterpolatorBase {
    public:
        FastCubicSplineInterpolator(double ts);

        FastCubicSplineInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;

    private:
        unsigned int position;
        bool isSetup;
        double times[CUBIC_SPLINE_WINDOW_SIZE];
        double measurements[CUBIC_SPLINE_WINDOW_SIZE];
        double M[CUBIC_SPLINE_WINDOW_SIZE];

        inline void setup();
        // Helper function for Thomas algorithm
        inline void solveTridiagonal();
            // const double* lower,    // sub-diagonal
            // const double* diag,     // diagonal  
            // const double* upper,    // super-diagonal
            // const double* rhs,      // right hand side
            // double* x,              // solution
            // int n);                 // size
        
        double h[CUBIC_SPLINE_WINDOW_SIZE-1];
        double slopes[CUBIC_SPLINE_WINDOW_SIZE-1];
        double diag[CUBIC_SPLINE_WINDOW_SIZE-2];
        double off_diag[CUBIC_SPLINE_WINDOW_SIZE-3];
        double d[CUBIC_SPLINE_WINDOW_SIZE-2];
        //double x[CUBIC_SPLINE_WINDOW_SIZE-2];

        double c_prime[CUBIC_SPLINE_WINDOW_SIZE-3];
        double d_prime[CUBIC_SPLINE_WINDOW_SIZE-2];

        int i;
};

