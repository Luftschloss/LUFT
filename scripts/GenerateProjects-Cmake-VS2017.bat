@echo off
pushd %~dp0\..\
call cmake -G"Visual Studio 15 2017" -Bbuild
popd
PAUSE