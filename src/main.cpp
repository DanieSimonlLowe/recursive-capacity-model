#include "socOcv/SocOcvCurveOptimizer.h"
#include "socOcv/PolySplineOcvSoc.h"


int main(int argc, char **argv) { 

    Eigen::VectorXd value(1);
    value[0] = 3;
    (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);
    value[0] = 5;
    (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);
    value[0] = 7;
    (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);
    value[0] = 9;
    (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);
    //(new NotKnotSplineSocOcvCurve())->display();
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 