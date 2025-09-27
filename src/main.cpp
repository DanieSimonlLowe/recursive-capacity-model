#include "socOcv/SocOcvCurveOptimizer.h"
#include "socOcv/SplineOcvSoc.h"
#include "algorithms/AkimaSpline.h"


int main(int argc, char **argv) { 

    (new SocOcvCurveOptimizer<SplineOcvSoc<AkimaSpline>>())->display();
    //(new NotKnotSplineSocOcvCurve())->display();
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 