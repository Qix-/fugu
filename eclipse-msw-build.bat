@echo off
echo Building project files for eclipse-msw
echo Making directory
mkdir build\eclipse-msw
echo Copying required dlls
copy lib\msw\*.dll build\eclipse-msw
echo Deleting cache and running cmake
cd build\eclipse-msw
del CMakeCache.txt
cmake -G "Eclipse CDT4 - MinGW Makefiles" ..\..\src\misc\iso
cd ..\..
echo Done!

