@echo off
setlocal

pushd %~dp0\..

call .\scripts\build.cmd --config release >nul
call build\Release\mzingacpp.exe %*

popd

endlocal

exit /b %ErrorLevel%
