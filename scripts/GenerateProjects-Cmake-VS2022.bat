@echo off
pushd %~dp0\..\
call cmake -G"Visual Studio 17 2022" -Bbuild
popd
PAUSE