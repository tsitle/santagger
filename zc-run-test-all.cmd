@REM ----------------------------------------------------------------------------
@REM Run all tests
@REM
@REM by TS, Aug 2025
@REM ----------------------------------------------------------------------------

@echo off
@REM set title of command window
title %0

@setlocal

set INT_ERROR_CODE=0

if exist cmake-build-manual-win-release goto checkInpFiles

echo Missing build directory >&2
echo. >&2

goto HaveError

:checkInpFiles

cd cmake-build-manual-win-release

set LTMP_TESTFILE_TXT=src\tests\data\data.txt
set LTMP_TESTFILE_BIN=src\tests\data\data.rnd

if exist ..\%LTMP_TESTFILE_TXT% goto checkInpFiles2

echo Could not find file '%LTMP_TESTFILE_TXT%' >&2
echo. >&2

goto HaveError

:checkInpFiles2

if exist ..\%LTMP_TESTFILE_BIN% goto startTests

echo Could not find file '%LTMP_TESTFILE_BIN%' >&2
echo. >&2

goto HaveError

:startTests

set LTMP_TESTFILE_TXT=..\%LTMP_TESTFILE_TXT%
set LTMP_TESTFILE_BIN=..\%LTMP_TESTFILE_BIN%

echo ----------------------------------------------------
echo TEST BINOBJ #1
echo ----------------------------------------------------
.\santagger_test_binobj %LTMP_TESTFILE_TXT%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST BINOBJ #2
echo ----------------------------------------------------
.\santagger_test_binobj %LTMP_TESTFILE_BIN%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST DL
echo ----------------------------------------------------
.\santagger_test_dl -s
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST M64
echo ----------------------------------------------------
.\santagger_test_m64
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST MTES
echo ----------------------------------------------------
.\santagger_test_mtes
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST STREAM
echo ----------------------------------------------------
.\santagger_test_stream -s
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST STRRD #1
echo ----------------------------------------------------
.\santagger_test_strrd %LTMP_TESTFILE_TXT%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST STRRD #2
echo ----------------------------------------------------
.\santagger_test_strrd %LTMP_TESTFILE_BIN%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST STRWR #1
echo ----------------------------------------------------
.\santagger_test_strwr %LTMP_TESTFILE_TXT%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST STRWR #2
echo ----------------------------------------------------
.\santagger_test_strwr %LTMP_TESTFILE_BIN%
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST SYSFILE
echo ----------------------------------------------------
.\santagger_test_sysfile
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST SYSFNC
echo ----------------------------------------------------
.\santagger_test_sysfnc
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo TEST TFLDMAP
echo ----------------------------------------------------
.\santagger_test_tfldmap
set INT_ERROR_CODE=%errorlevel%
if %INT_ERROR_CODE% neq 0 goto HaveError

echo.
echo ----------------------------------------------------
echo ALL TEST DONE :)
echo ----------------------------------------------------

goto TheEnd

:HaveError

echo. >&2
echo Aborting >&2
echo. >&2

set INT_ERROR_CODE=1

:TheEnd

exit /B %INT_ERROR_CODE%
