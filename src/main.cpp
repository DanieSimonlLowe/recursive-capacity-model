#include "Algorithms/recursiveLeastSquares.h"
#include "Algorithms/splines/FritschCarlsonSpline.h"
#include "BatteryModels/DirectBatteryModel.h"
#include "CapacityEstimation/RlsCapacityEstimator.h"
#include "Interpolation/CubicSplineInterpolator.h"
#include "OcvEstimation/Rls1EcmOcvEstimator.h"
#include "OcvToSoc/SplineOcvToSoc.h"
#include "Optimization/BayesianOptimize.h"

int main(int argc, char **argv) {
    BayesianOptimize<DirectBatteryModel<
        CubicSplineInterpolator, CubicSplineInterpolator, SplineOcvToSoc<FritschCarlsonSpline>,
        RlsCapacityEstimator<RecursiveLeastSquares>, Rls1EcmOcvEstimator<RecursiveLeastSquares>>>(
        2.1, ErrorMetric::CapacityError, true);
}

//(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults();

// (new SocToOcvOptimizer<PolySplineOcvToSoc>())->display(value);

// BayesianOptimize<StateBasedBatteryModel<
// Rls2EcmStateEstimator<RecursiveLeastSquares>, CubicSplineInterpolator,
// CubicSplineInterpolator, SplineSocToOcv<AkimaSpline>, SplineOcvToSoc<AkimaSpline>,
// EkfSocEstimator, RlsCapacityEstimator<RecursiveLeastSquares>,
// Rls1EcmOcvEstimator<RecursiveLeastSquares>>>(2.1, ErrorMetric::VoltageError, true);
