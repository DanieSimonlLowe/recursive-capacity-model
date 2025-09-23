#include "loading/readOcvSocCSV.h"

OcvSocData readOcvSocCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::vector<double> socData;
    std::vector<double> voltData;
    std::string line;
    bool isFirstLine = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        std::stringstream ss(line);
        std::string socStr, voltStr;
        
        // Parse comma-separated values
        if (std::getline(ss, socStr, ',') && std::getline(ss, voltStr)) {
            try {
                double soc = std::stod(socStr) / 100; // is in percent
                double volt = std::stod(voltStr);
                
                socData.push_back(soc);
                voltData.push_back(volt);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not parse line: " << line << std::endl;
                continue;
            }
        }
    }
    
    file.close();

    if (socData.empty()) {
        throw std::runtime_error("No valid data found in file");
    }
    
    // Convert vectors to Eigen vectors
    OcvSocData result;
    result.soc = Eigen::Map<Eigen::VectorXd>(socData.data(), socData.size());
    result.ocv = Eigen::Map<Eigen::VectorXd>(voltData.data(), voltData.size());

    return result;
}