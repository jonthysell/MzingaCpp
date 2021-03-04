#!/bin/bash

pushd $(dirname $0)/..

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

popd