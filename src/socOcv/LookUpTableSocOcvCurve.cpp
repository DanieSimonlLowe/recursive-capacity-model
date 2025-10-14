#include "socOcv/LookUpTableSocOcvCurve.h"

LookUpTableSocOcvCurve::LookUpTableSocOcvCurve(const OcvSocData &data, const Eigen::VectorXd& params) {
    socs = data.soc;
    ocvs = data.ocv;
}

double LookUpTableSocOcvCurve::getOcv(double soc) {
    int closestIndex;
    (socs.array() - soc).abs().minCoeff(&closestIndex);
    return ocvs(closestIndex);
}

double LookUpTableSocOcvCurve::getOcvSocDerivative(double soc) {
    int closestIndex;
    (socs.array() - soc).abs().minCoeff(&closestIndex);
    
    if (closestIndex == 0) {
        closestIndex++; 
    }
    if (closestIndex == socs.size() - 1) {
        closestIndex--;
    }

    const double socl = socs(closestIndex-1);
    const double ocvl = ocvs(closestIndex-1);

    const double socm = socs(closestIndex);
    const double ocvm = ocvs(closestIndex);

    const double socr = socs(closestIndex+1);
    const double ocvr = ocvs(closestIndex+1);

    const double h1 = socm - socl;  // spacing between left and middle points
    const double h2 = socr - socm;  // spacing between middle and right points
    
    // Three-point finite difference for non-uniform grid
    const double derivative = (ocvl * (-h2) + ocvm * (h2 - h1) + ocvr * h1) / (h1 * h2 * (h1 + h2));
    return derivative;
}