#include "socOcv/SocOcvCurveOptimizer.h"
#include "socOcv/LogNegPolyLsSocOcvCurve.h"


int main(int argc, char **argv) { 

    (new SocOcvCurveOptimizer<LogNegPolyLsSocOcvCurve>())->optimize();
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 