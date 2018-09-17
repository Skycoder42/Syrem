@echo off
setlocal

set qtplatform=%PLATFORM%
for %%* in (.) do set CurrDirName=%%~nx*

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 || exit /B 1

dir /s install\
