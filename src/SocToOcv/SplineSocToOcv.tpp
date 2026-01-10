
template <typename T>
SplineSocToOcv<T>::SplineSocToOcv(const OcvSocData &data, const Eigen::VectorXd &params) {
    spline = new T(data.soc, data.ocv);
}

template <typename T> double SplineSocToOcv<T>::getOcv(double soc) { return spline->predict(soc); }

template <typename T> double SplineSocToOcv<T>::getOcvToSocDerivative(double soc) {
    return spline->derivative(soc);
}
