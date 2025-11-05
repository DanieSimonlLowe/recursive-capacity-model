

Setup instructions:
cd /mnt/c/Users/Danie/Desktop/project/build

cmake .. -G "Unix Makefiles"

cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make all
./tests/EVLifeCycle_test

./EVLifeCycle_main

0.269693,1,9.65345e-05,0.999982


Notes:

TODO do this later cause I want to try it. (Is a 2ECM model.)
https://www.mdpi.com/1996-1073/14/4/1054



// TODO OCVEKFEstimator that uses EKF with other stuff
// This reqires a good soc ocv model.
// So do the soc ocv model evaluation first. 