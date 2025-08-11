#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/twoPointLinearInterpolator.h"
#include "interpolation/polySineEKFInterpolator.h"



int main(int argc, char **argv) { 
    (new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    (new InterpolatorEvaluator(new TwoPointLinearInterpolator(0.1)))->printResults(); 
    // TODO later make it so that I select which thing I am evaluating.
}