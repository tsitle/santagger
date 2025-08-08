@REM ----------------------------------------------------------------------------
@REM Build the library and all executables
@REM
@REM This script should be run from a MSYS2 UCRT64 terminal.
@REM Otherwise CMake might not be able to find the MinGW libraries
@REM required for this project.
@REM
@REM by TS, Aug 2025
@REM ----------------------------------------------------------------------------

@echo off
@REM set title of command window
title %0

@setlocal

set INT_ERROR_CODE=0

if exist cmake-build-manual-win-release goto startBuild

mkdir cmake-build-manual-win-release

:startBuild

cd cmake-build-manual-win-release

cmake --fresh -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D STRIP_AFTER_BUILD=ON -D BUILD_SHARED_LIBS=OFF -S .. -B .
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_app
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_binobj
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_dl
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_m64
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_mtes
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_stream
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_strrd
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_strwr
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_sysfile
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_sysfnc
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

cmake --build . --parallel 4 --target santagger_test_tfldmap
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

goto TheEnd

:HaveError

echo. >&2
echo Aborting >&2
echo. >&2

set INT_ERROR_CODE=1

:TheEnd

exit /B %INT_ERROR_CODE%
