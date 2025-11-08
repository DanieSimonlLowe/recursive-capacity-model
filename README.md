# Recursive Capacity Estimation Project
## Overview

The project estimates the remaining battery capacity over time by applying recursive estimation Algorithms to voltage, current, and temperature data from the NASA battery dataset. It compares different model-based approaches such as recursive least squares, Kalman filters, and weighted total least squares.

The key focus is on:
* Exploring fast Algorithms for capacity/state estimation.
* Avoiding deep learning and instead using model-based and statistical methods.
* Thoroughly testing subcomponents and comparing multiple approaches.
* Working with equivalent circuit models (ECMs) and Interpolation techniques.

Disclaimer: This project is for educational and experimental purposes only. It is not intended for commercial use.

## Project Structure
src/

 ├─ Algorithms/          # Core estimation and filtering Algorithms

 ├─ CapacityEstimation/   # Capacity estimation methods from state of charge current measurements. 

 ├─ EcmStateEstimation/   # Equivalent circuit model estimation methods

 ├─ Interpolation/       # Interpolators and evaluation tools

 ├─ Loading/             # Data Loading & NASA dataset processing

 ├─ Optimization/        # Hyperparameter Optimization

 ├─ SocEstimation/       # State of charge estimation from measurements of voltage and current

 └─ SocOcv/     # State of charge from open circuit voltage models.

## Dependencies
- [Eigen](https://eigen.tuxfamily.org/) (linear algebra)  
- [BayesOpt](http://rmcantin.bitbucket.io/html/) (Bayesian Optimization)  
- [MATIO](https://github.com/tbeu/matio) (reading NASA .mat cycle files)  

## Build
This project is written in C++17. You can build it with CMake:
```
mkdir build && cd build
cmake ..
make all
```

## Learning Goals
This project is intended as a sandbox for experimenting with recursive estimation techniques, particularly:
* Understanding the strengths and weaknesses of model-based capacity estimation.
* Exploring fast Algorithms beyond deep learning.
* Building a modular framework for testing new estimation methods.

