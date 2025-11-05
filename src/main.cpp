#include "BatteryModel.h"
#include "socOcv/SplineOcvSoc.h"
#include "algorithms/splines/AkimaSpline.h"
#include "algorithms/recursiveLeastSquares.h"
#include "EcmStateEstimation/Rls2EcmStateEstimator.h"
#include "interpolation/fastCubicSplineInterpolator.h"
#include "algorithms/recursiveLeastSquares.h"
#include "socOcv/SplineOcvSoc.h"
#include "SocEstimation/EkfSocEstimator.h"
#include "CapacityEstimation/RlsCapacityEstimator.h"


int main(int argc, char **argv) {

    Eigen::VectorXd value = Eigen::VectorXd::Zero(50);

    BatteryModel<Rls2EcmStateEstimator<RecursiveLeastSquares>,
    FastCubicSplineInterpolator,FastCubicSplineInterpolator,
    SplineOcvSoc<AkimaSpline>,EkfSocEstimator,
    RlsCapacityEstimator<RecursiveLeastSquares>> model(2,value);
    
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // (new SocOcvCurveOptimizer<PolySplineOcvSoc>())->display(value);