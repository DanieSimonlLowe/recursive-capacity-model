#include "loading/processNasaCycles.h"


Eigen::VectorXd toVector(matvar_t *var) {
    size_t length = var->dims[1]; // or [0] depending on MAT format
    double* data = static_cast<double*>(var->data);
    Eigen::VectorXd vector(length);
    for (size_t j = 0; j < length; ++j) {
        vector(j) = data[j];
    }
    return vector;
}

// Process NASA .mat file and dispatch to handler
// https://data.nasa.gov/dataset/li-ion-battery-aging-datasets
void processNasaCycles(const std::string &matFile, const std::string &varbleName, CycleHandler &handler) {
    mat_t *mat = Mat_Open(matFile.c_str(), MAT_ACC_RDONLY);
    if (!mat) {
        throw std::runtime_error("Cannot open MAT file: " + matFile);
    }

    // Read the top-level structure
    matvar_t *file = Mat_VarRead(mat, varbleName.c_str());
    if (!file || file->class_type != MAT_C_STRUCT) {
        Mat_VarFree(file);
        Mat_Close(mat);
        throw std::runtime_error("Unexpected matfile format");
    }

    // Access the 'cycle' field
    matvar_t *cyclesVar = Mat_VarGetStructFieldByName(file, "cycle", 0);
    
    if (!cyclesVar || (cyclesVar->class_type != MAT_C_CELL && cyclesVar->class_type != MAT_C_STRUCT)) {
        Mat_VarFree(file);
        Mat_Close(mat);
        throw std::runtime_error("Missing or invalid 'cycle' field");
    }

    size_t num_elements = cyclesVar->dims[1];
    std::string cycleType;
    matvar_t *typeVar = nullptr;
    matvar_t *dataVar = nullptr;

    matvar_t *voltVar = nullptr;
    matvar_t *currVar = nullptr;
    matvar_t *tempVar = nullptr;
    matvar_t *timeVar = nullptr;
    
    matvar_t *capacityVar = nullptr;

    matvar_t *impVar = nullptr;
    matvar_t *risVar = nullptr;
    matvar_t *recVar = nullptr;
    
    for (size_t i = 0; i < num_elements; ++i) {
        
        typeVar = Mat_VarGetStructFieldByName(cyclesVar, "type", i);
        char *ctype = static_cast<char*>(typeVar->data);
        cycleType = std::string(ctype);
        dataVar = Mat_VarGetStructFieldByName(cyclesVar, "data", i);

        if (cycleType == "charge" || cycleType == "discharge") { 
            voltVar = Mat_VarGetStructFieldByName(dataVar, "Voltage_measured", 0);
            currVar = Mat_VarGetStructFieldByName(dataVar, "Current_measured", 0);
            tempVar = Mat_VarGetStructFieldByName(dataVar, "Temperature_measured", 0);
            timeVar = Mat_VarGetStructFieldByName(dataVar, "Time", 0);
            

            if (cycleType == "discharge") {
                capacityVar = Mat_VarGetStructFieldByName(dataVar, "Capacity", 0);

                handler.onDischarge(
                    toVector(voltVar),
                    toVector(currVar),
                    toVector(tempVar),
                    toVector(timeVar),
                    ((double*)capacityVar->data)[0]
                );
            } else {
                handler.onCharge(
                    toVector(voltVar),
                    toVector(currVar),
                    toVector(tempVar),
                    toVector(timeVar)
                );
            }
            
        } else if (cycleType == "impedance") {
            impVar = Mat_VarGetStructFieldByName(dataVar, "Rct", 0);
            risVar = Mat_VarGetStructFieldByName(dataVar, "Re", 0);

            handler.onImpedance(
                ((double*)impVar->data)[0],
                ((double*)risVar->data)[0]
            );
        }
        else {
            std::cerr << "Invalid cycle type: " << cycleType << std::endl;
        }
    }
    
    Mat_VarFree(file);
    Mat_Close(mat);
}
