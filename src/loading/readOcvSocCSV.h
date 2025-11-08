#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

struct OcvSocData {
    Eigen::VectorXd soc;
    Eigen::VectorXd ocv;
};

OcvSocData ReadOcvSocCSV(const std::string& filename);