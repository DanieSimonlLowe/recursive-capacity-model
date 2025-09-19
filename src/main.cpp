#include "socOcv/SocOcvCurveOptimizable.h"
#include "socOcv/CubicSplineSocOcvCurve.h"

#include <chrono>
int main(int argc, char **argv) { 

    (new SocOcvCurveOptimizable<CubicSplineSocOcvCurve>())->display();
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 