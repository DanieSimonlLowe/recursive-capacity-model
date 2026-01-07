#include "BatteryModels/StateBasedBatteryModel.h"
#include "SocOcv/SplineOcvSoc.h"
#include "Algorithms/splines/AkimaSpline.h"
#include "Algorithms/RecursiveLeastSquares.h"
#include "EcmStateEstimation/Rls2EcmStateEstimator.h"
#include "Interpolation/CubicSplineInterpolator.h"
#include "SocEstimation/EkfSocEstimator.h"
#include "CapacityEstimation/RlsCapacityEstimator.h"
#include "Optimization/BayesianOptimize.h"
#include "OcvEstimation/Rls1EcmOcvEstimator.h"

int main(int argc, char **argv) {

    BayesianOptimize<
    StateBasedBatteryModel<Rls2EcmStateEstimator<RecursiveLeastSquares>,
    CubicSplineInterpolator,CubicSplineInterpolator,
    SplineOcvSoc<AkimaSpline>,EkfSocEstimator,
    RlsCapacityEstimator<RecursiveLeastSquares>,
    Rls1EcmOcvEstimator<RecursiveLeastSquares>
    >>(2.1,ErrorMetric::VoltageError,true);
    
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);
