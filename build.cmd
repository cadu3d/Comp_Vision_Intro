@echo off
setlocal

set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if exist "%VCVARS%" goto vcvars_found
echo vcvars64.bat nao encontrado em: %VCVARS%
exit /b 1

:vcvars_found

call "%VCVARS%"
if errorlevel 1 exit /b %errorlevel%

cmake --build .\cmake-build-debug
exit /b %errorlevel%
