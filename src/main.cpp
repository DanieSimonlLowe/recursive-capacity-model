#include "interpolation/InterpolatorEvaluator.h"
#include "interpolation/AkimaSplineInterpolator.h"


int main(int argc, char **argv) { 

    (new InterpolatorEvaluator(new AkimaSplineInterpolator(0,50)))->printResults(); 
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);