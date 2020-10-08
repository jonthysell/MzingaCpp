@echo off
setlocal

pushd %~dp0

call build\Debug\mzingacpp.exe %*

popd

endlocal

exit /b %ErrorLevel%
