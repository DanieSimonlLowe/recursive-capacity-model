#include "Algorithms/splines/FritschCarlsonSpline.h"
#include "OcvToSoc/SplineOcvToSoc.h"
#include "OcvToSoc/OcvToSocOptimizer.h"

int main(int argc, char **argv) {
    OcvToSocOptimizer<SplineOcvToSoc<FritschCarlsonSpline>> *optimizer =
        new OcvToSocOptimizer<SplineOcvToSoc<FritschCarlsonSpline>>();
    optimizer->display();
}

//(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults();

// (new SocToOcvOptimizer<PolySplineOcvToSoc>())->display(value);

// BayesianOptimize<StateBasedBatteryModel<
// Rls2EcmStateEstimator<RecursiveLeastSquares>, CubicSplineInterpolator,
// CubicSplineInterpolator, SplineSocToOcv<AkimaSpline>, SplineOcvToSoc<AkimaSpline>,
// EkfSocEstimator, RlsCapacityEstimator<RecursiveLeastSquares>,
// Rls1EcmOcvEstimator<RecursiveLeastSquares>>>(2.1, ErrorMetric::VoltageError, true);
