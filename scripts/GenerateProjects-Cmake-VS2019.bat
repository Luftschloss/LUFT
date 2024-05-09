@echo off
pushd %~dp0\..\
call cmake -G"Visual Studio 16 2019" -Bbuild
popd
PAUSE