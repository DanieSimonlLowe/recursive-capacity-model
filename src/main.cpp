#include "BatteryModel.h"
#include "SocOcv/SplineOcvSoc.h"
#include "Algorithms/splines/AkimaSpline.h"
#include "Algorithms/RecursiveLeastSquares.h"
#include "EcmStateEstimation/Rls2EcmStateEstimator.h"
#include "Interpolation/FastCubicSplineInterpolator.h"
#include "SocEstimation/EkfSocEstimator.h"
#include "CapacityEstimation/RlsCapacityEstimator.h"
#include "Optimization/BayesianOptimize.h"

int main(int argc, char **argv) {

    BayesianOptimize<
    BatteryModel<Rls2EcmStateEstimator<RecursiveLeastSquares>,
    FastCubicSplineInterpolator,FastCubicSplineInterpolator,
    SplineOcvSoc<AkimaSpline>,EkfSocEstimator,
    RlsCapacityEstimator<RecursiveLeastSquares>>
    >(2.1,ErrorMetric::ResistanceError);
    
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);