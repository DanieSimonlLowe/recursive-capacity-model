

template<typename RLS>
Rls2EcmStateEstimator<RLS>::Rls2EcmStateEstimator(Eigen::VectorXd& params) : 
    {
    stateCalculated = false;
    deltaTime = 0;
    rls = new RLS(5);
    rls->setParams(params);
}

template<typename RLS>
void Rls2EcmStateEstimator<RLS>::update(const std::vector<double> current, 
                    const std::vector<double> voltage) {
    size_t n = current.size(); // all are same length
    for (size_t i = 2; i < n; ++i) {
        // Do something with (c, v, t)
        Eigen::VectorXd input(5);
        input << voltage[i-1], voltage[i-2], current[i], current[i-1], current[i-2];
        rls->update(input,voltage[i]);
    }
    stateCalculated = false;
}

// Equations and derivations can be found in this paper.
// https://www.mdpi.com/1996-1073/12/12/2242
// a: ohmic resistance term
// b, c: coefficients of quadratic for time constants
// d, f: intermediate variables for RC branch resistances
template<typename RLS>
void Rls2EcmStateEstimator<RLS>::calculateState() {
    Eigen::VectorXd weights =  rls->getState();
    double a = (weights[3] - weights[2] - weights[4]) / (1 + weights[0] - weights[1]);
    double b = deltaTime * deltaTime * (1 + weights[0] - weights[1]) / (4 * (1 - weights[0] - weights[1]));
    double c = deltaTime * (1 + weights[1]) / (1 - weights[0] - weights[1]);
    double d = (weights[2] + weights[3] + weights[4]) / (weights[0] + weights[1] - 1);
    double f = deltaTime * (weights[4] - weights[3]) / (1 - weights[0] - weights[1]);

    double root = std::sqrt(c*c - 4 * b);
    double T1 = 0.5*(c + root);
    double T2 = 0.5*(c - root);

    R0 = a;
    R1 = (T1 * (d-1) + a*c - f) / root;
    R2 = d - a - R1;
    C1 = T1/R1;
    C2 = T2/R2;
    stateCalculated = true;
}

template<typename RLS>
bool Rls2EcmStateEstimator<RLS>::canCalculateState() const {
    Eigen::VectorXd weights =  rls->getState();
    if (weights[0] - weights[1] == -1) return false;
    if (weights[0] + weights[1] == 1) return false;

    double b_top = deltaTime * deltaTime * (1 + weights[0] - weights[1]);
    double c_top = deltaTime * (1 + weights[1]);
    double c_bottom = (1 - weights[0] - weights[1]); // Same as b_bottom

    /*
    for std::sqrt(c*c - 4 * b) to be defined and not zero c*c > 4*b
    as 4*b_bottom = c_bottom (by defintion) 

    (c_top/c_bottom)*(c_top/c_bottom) > 4*(b_top/(4*c_bottom))
    (c_top/c_bottom)*(c_top/c_bottom) > b_top/c_bottom
    (c_top*c_top)/(c_bottom*c_bottom) > b_top/c_bottom
    (c_top*c_top) > b_top*c_bottom (as c_bottom then c_bottom*c_bottom is postive and has no effect on sign.)
    */

    if (c_top*c_top <= b_top * c_bottom) return false; // root of zero is zero and there is divide by root.


    // needs to be postive and if mult of top and bottom is postive then div is also postive.
    // check that R0 is not 0 or neggtive
    if ((weights[3] - weights[2] - weights[4])*(1 + weights[0] - weights[1]) <= 0) return false;

    // I don't do any checks that would reqire a sqrt.

    return true;
}

template<typename RLS>
double Rls2EcmStateEstimator<RLS>::getOhmicResistance() {
    if (!stateCalculated) {
        calculateState();
    }
    return R0;
}

template<typename RLS>
const std::vector<double> Rls2EcmStateEstimator<RLS>::getBranchResistances() {
    if (!stateCalculated) {
        calculateState();
    }
    return {R1, R2};
}

template<typename RLS>
const std::vector<double> Rls2EcmStateEstimator<RLS>::getBranchCapacities() {
    if (!stateCalculated) {
        calculateState();
    }
    return {C1, C2};
}


template<typename RLS>
void Rls2EcmStateEstimator<RLS>::setDeltaTime(double deltaTime) {
    this->deltaTime = deltaTime;
}

template<typename RLS>
static size_t Rls2EcmStateEstimator<RLS>::getParamsCount() {
    RLS::getParamsCount();
}

template<typename RLS>
static const Eigen::VectorXd Rls2EcmStateEstimator<RLS>::getLowerBounds() {
    RLS::getLowerBounds();
}

template<typename RLS>
static const Eigen::VectorXd Rls2EcmStateEstimator<RLS>::getUpperBounds() {
    RLS::getUpperBounds();
}

template<typename RLS>
static const int Rls2EcmStateEstimator<RLS>::getDimension() {
    return 2;
}