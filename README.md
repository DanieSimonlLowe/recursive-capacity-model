# Recursive Capacity Estimation Project
## Overview

This project is a personal research and learning project focused on developing and testing methods for battery capacity estimation using recursive algorithms and circuit-model-based approaches.

The key focus is on:
* Exploring fast algorithms for capacity/state estimation.
* Avoiding deep learning and instead using model-based and statistical methods.
* Thoroughly testing subcomponents and comparing multiple approaches.
* Working with equivalent circuit models (ECMs) and interpolation techniques.

Disclaimer: This project is for educational and experimental purposes only. It is not intended for commercial use.

## Project Structure
src/

 ├─ algorithms/          # Core estimation and filtering algorithms

 ├─ interpolation/       # Interpolators and evaluation tools

 ├─ loading/             # Data loading & NASA dataset processing

 ├─ optimization/        # Hyperparameters Optimization

 └─ stateEstimation/     # Circuit model based state estimators

## Dependencies
- [Eigen](https://eigen.tuxfamily.org/) (linear algebra)  
- [BayesOpt](http://rmcantin.bitbucket.io/html/) (Bayesian optimization)  
- [MATIO](https://github.com/tbeu/matio) (reading NASA .mat cycle files)  
- [NFFT3](https://www-user.tu-chemnitz.de/~potts/nfft/) (non-uniform FFT for frequency analysis)

## Build
This project is written in C++17. You can build it with CMake:
`
mkdir build && cd build
cmake ..
make all
`

## Learning Goals
This project is intended as a sandbox for experimenting with recursive estimation techniques, particularly:
* Understanding the strengths and weaknesses of model-based capacity estimation.
* Exploring fast algorithms beyond deep learning.
* Building a modular framework for testing new estimation methods.

