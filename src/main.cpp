#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/twoPointLinearInterpolator.h"
#include "interpolation/bayesianOptimizeMSE.h"


int main(int argc, char **argv) { 

    (new InterpolatorEvaluator(new TwoPointLinearInterpolator(0.1)))->printResults();

    // PolyRSLInterpolator interpolator(0.1, 1, 1, 1, 1);
    // bayesianOptimizeMSE(interpolator,MSEType::Voltage,interpolator.getLowerBounds(),interpolator.getUpperBounds());
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 