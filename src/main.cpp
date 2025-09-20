#include "socOcv/SocOcvCurveOptimizer.h"
#include "socOcv/LeastSquaresSocOcvCurve.h"


int main(int argc, char **argv) { 

    (new SocOcvCurveOptimizer<LeastSquaresSocOcvCurve>())->optimize();
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 