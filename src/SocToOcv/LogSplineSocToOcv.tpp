
template <typename T>
LogSplineSocToOcv<T>::LogSplineSocToOcv(const OcvSocData &data, const Eigen::VectorXd &params) {

    spline = new T(data.soc.array().log(), data.ocv);
}

template <typename T> double LogSplineSocToOcv<T>::getOcv(double soc) {
    return spline->predict(std::log(soc));
}

template <typename T> double LogSplineSocToOcv<T>::getOcvToSocDerivative(double soc) {
    return spline->derivative(std::log(soc)) / soc;  // Apply chain rule
}
