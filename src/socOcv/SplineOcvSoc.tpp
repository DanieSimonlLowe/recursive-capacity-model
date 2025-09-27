
template<typename T>
SplineOcvSoc<T>::SplineOcvSoc(const OcvSocData &data, const Eigen::VectorXd& params) {
    spline = new T(data.soc, data.ocv);
}

template<typename T>
double SplineOcvSoc<T>::getOcv(double soc) {
    return spline->predict(soc);
}

template<typename T>
double SplineOcvSoc<T>::getOcvSocDerivative(double soc) {
    return spline->derivative(soc);
}