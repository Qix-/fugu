@echo off
echo Building project files for eclipse-msw
mkdir build-eclipse-msw-release
echo Deleting cache and re-running cmake
cd build-eclipse-msw-release
del CMakeCache.txt
cmake -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE:STRING=Release ..\src
cd ..
echo Done!
