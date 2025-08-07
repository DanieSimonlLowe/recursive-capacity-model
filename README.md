cd /mnt/c/Users/Danie/Desktop/project/build

cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make all
./tests/EVLifeCycle_test

./EVLifeCycle_main