

template<typename RLS>
Rls2EcmStateEstimator<RLS>::Rls2EcmStateEstimator(const Eigen::VectorXd& params) {
    stateCalculated = false;
    deltaTime = 0;
    rls = new RLS(5);
    rls->setParams(params);
}

template<typename RLS>
void Rls2EcmStateEstimator<RLS>::update(const Eigen::VectorXd& current, 
                    const Eigen::VectorXd& voltage) {
    size_t n = current.size(); // all are same length
    if (n > 2) {
        for (size_t i = 2; i < n; ++i) {
            // Do something with (c, v, t)
            Eigen::VectorXd input(5);
            input << voltage[i-1], voltage[i-2], -current[i], -current[i-1], -current[i-2];
            rls->update(input,voltage[i]);
        }
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
    const double denom1 = 1 + weights[0] - weights[1];
    const double denom2 = 1 - weights[0] - weights[1];
    const double a = (weights[3] - weights[2] - weights[4]) / denom1;
    const double b = deltaTime * deltaTime * denom1 / (4 * denom2);
    const double c = deltaTime * (1 + weights[1]) / denom2;
    const double d = -(weights[2] + weights[3] + weights[4]) / denom2;
    const double f = deltaTime * (weights[4] - weights[2]) / denom2;

    const double root = std::sqrt(c*c - 4 * b);
    const double T1 = 0.5*(c + root);
    const double T2 = 0.5*(c - root);

    R0 = a;
    R1 = (T1 * (d-a) + a*c - f) / root;
    R2 = d - a - R1;
    C1 = T1/R1;
    C2 = T2/R2;
    stateCalculated = true;
}

template<typename RLS>
bool Rls2EcmStateEstimator<RLS>::canCalculateState() const {
    Eigen::VectorXd weights =  rls->getState();
    const double esp = 1e-10;


    const double denom1 = 1 + weights[0] - weights[1];
    const double denom2 = 1 - weights[0] - weights[1];
    if (std::abs(denom1) <= esp) return false;
    if (std::abs(denom2) <= esp) return false;
    
    const double a = (weights[3] - weights[2] - weights[4]) / denom1;
    const double b = deltaTime * deltaTime * denom1 / (4 * denom2);
    const double c = deltaTime * (1 + weights[1]) / denom2;
    const double d = -(weights[2] + weights[3] + weights[4]) / denom2;
    const double f = deltaTime * (weights[4] - weights[2]) / denom2;
    
    
    if (a < esp) return false; // a == R0

    if (c*c - 4*b <= esp) return false;
    const double root = std::sqrt(c*c - 4 * b);
    if (root <= esp) return false; // roots can't be negitive so don't need to do abs.
    const double T1 = 0.5*(c + root);
    const double T2 = 0.5*(c - root);
    
    const double R1 = (T1 * (d-a) + a*c - f) / root;
    if (R1 <= esp) return false;
    const double R2 = d - a - R1;
    if (R2 <= esp) return false;

    const double C1 = T1/R1;
    if (C1 <= esp) return false;
    const double C2 = T2/R2;
    if (C2 <= esp) return false;

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
size_t Rls2EcmStateEstimator<RLS>::getParamsCount() {
    return RLS::getParamsCount();
}

template<typename RLS>
const Eigen::VectorXd Rls2EcmStateEstimator<RLS>::getLowerBounds() {
    return RLS::getLowerBounds();
}

template<typename RLS>
const Eigen::VectorXd Rls2EcmStateEstimator<RLS>::getUpperBounds() {
    return RLS::getUpperBounds();
}

template<typename RLS>
const int Rls2EcmStateEstimator<RLS>::getDimension() {
    return 2;
}