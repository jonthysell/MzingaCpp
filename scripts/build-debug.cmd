@echo off
setlocal

pushd %~dp0\..

mkdir build 2>&1
cd build

cmake ..
cmake --build . --config Debug

popd

endlocal

exit /b %ErrorLevel%
