@echo off
setlocal

pushd %~dp0\..

call .\scripts\build.cmd >nul
call build\Debug\mzingacpp.exe %*

popd

endlocal

exit /b %ErrorLevel%
