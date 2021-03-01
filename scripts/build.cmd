@echo off
setlocal

pushd %~dp0\..

mkdir build
cd build

cmake ..
cmake --build . %*

popd

endlocal

exit /b %ErrorLevel%
