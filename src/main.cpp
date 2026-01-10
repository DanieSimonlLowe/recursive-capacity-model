#include "Algorithms/RecursiveLeastSquares.h"
#include "Algorithms/splines/AkimaSpline.h"
#include "BatteryModels/StateBasedBatteryModel.h"
#include "CapacityEstimation/RlsCapacityEstimator.h"
#include "EcmStateEstimation/Rls2EcmStateEstimator.h"
#include "Interpolation/CubicSplineInterpolator.h"
#include "OcvEstimation/Rls1EcmOcvEstimator.h"
#include "OcvToSoc/SplineOcvToSoc.h"
#include "Optimization/BayesianOptimize.h"
#include "SocEstimation/EkfSocEstimator.h"
#include "SocToOcv/SplineSocToOcv.h"

int main(int argc, char **argv) {

    BayesianOptimize<StateBasedBatteryModel<
        Rls2EcmStateEstimator<RecursiveLeastSquares>, CubicSplineInterpolator,
        CubicSplineInterpolator, SplineSocToOcv<AkimaSpline>, SplineOcvToSoc<AkimaSpline>,
        EkfSocEstimator, RlsCapacityEstimator<RecursiveLeastSquares>,
        Rls1EcmOcvEstimator<RecursiveLeastSquares>>>(2.1, ErrorMetric::VoltageError, true);
}

//(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults();

// (new SocToOcvOptimizer<PolySplineOcvToSoc>())->display(value);
