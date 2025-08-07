#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/twoPointLinearInterpolator.h"



int main(int argc, char **argv) { 
    (new InterpolatorEvaluator(new TwoPointLinearInterpolator(0.1)))->printResults(); 
    // TODO later make it so that I select which thing I am evaluating.
}