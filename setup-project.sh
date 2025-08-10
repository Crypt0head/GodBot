#!/bin/bash

starttime=$(date +"%s")

export LD_LIBRARY_PATH=/usr/local/lib

conan install . --output-folder=build --build=missing

cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .

endtime=$(date +"%s")

echo "Time: $(( endtime - starttime )) seconds"