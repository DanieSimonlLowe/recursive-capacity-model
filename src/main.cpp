#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/cubicSplineInterpolator.h"
#include "interpolation/fastCubicSplineInterpolator.h"
#include "interpolation/bayesianOptimizeMSE.h"

#include <chrono>
int main(int argc, char **argv) { 

    auto start = std::chrono::high_resolution_clock::now();
    //std::cout << "start \n";
    (new InterpolatorEvaluator(new FastCubicSplineInterpolator(0.1)))->printResults();

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    // Secound
    start = std::chrono::high_resolution_clock::now();
    //std::cout << "start \n";
    (new InterpolatorEvaluator(new CubicSplineInterpolator(0.1,50)))->printResults();

    end = std::chrono::high_resolution_clock::now();

    duration = end - start;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;


    // PolyRSLInterpolator interpolator(0.1, 1, 1, 1, 1);
    // bayesianOptimizeMSE(interpolator,MSEType::Voltage,interpolator.getLowerBounds(),interpolator.getUpperBounds());
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    // 