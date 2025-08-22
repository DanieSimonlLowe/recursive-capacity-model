#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/notKnotSplineInterpolator.h"
#include "interpolation/bayesianOptimizeMSE.h"

#include <chrono>
int main(int argc, char **argv) { 

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << 10 << " \n";
    (new InterpolatorEvaluator(new NotKnotSplineInterpolator(0.1,10)))->printResults();

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;


    // PolyRSLInterpolator interpolator(0.1, 1, 1, 1, 1);
    // bayesianOptimizeMSE(interpolator,MSEType::Voltage,interpolator.getLowerBounds(),interpolator.getUpperBounds());
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 