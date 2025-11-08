#include "BatteryModel.h"
#include "SocOcv/SplineOcvSoc.h"
#include "Algorithms/splines/AkimaSpline.h"
#include "Algorithms/RecursiveLeastSquares.h"
#include "EcmStateEstimation/Rls2EcmStateEstimator.h"
#include "Interpolation/FastCubicSplineInterpolator.h"
#include "Algorithms/RecursiveLeastSquares.h"
#include "SocOcv/SplineOcvSoc.h"
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