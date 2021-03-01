@echo off
setlocal

pushd %~dp0\..

mkdir build 2>&1
cd build

cmake ..
cmake --build . %*

popd

endlocal

exit /b %ErrorLevel%
