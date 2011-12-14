fg/fugu readme

fg/fugu is a system for procedurally generating animated geometric forms in real-time. A pre-compiled fugu executable can be found online for windows and mac. Only try to compile the source if you know what you are doing. :)

Building Instructions
=====================
First make sure you have CMake 2.8

fg uses CMake to build. As such, it should be possible to output project files for Xcode, Visual Studio, etc. Some build scripts are provided for some of these, to generate other projects you will need to write your own scripts (patches welcome!).


XCode
-----
> ./xcode-build.sh
> open build-xcode/FG.xcodeproj
> Select "install" target and run

OSX/Make
--------
> ./osx-make-build.sh
> cd build-osx-make
> make install

Windows/MinGW/Eclipse
---------------------
> eclipse-msw-build.bat
> cd build-eclipse-msw
> make install

Directory Structure
===================
src/ contains the source for fugu and the cmake build script
build-*/ will contain the build files after running a batch file
include/ contains headers for dependencies
scripts/ contains the fg lua scripts
lib/ contains pre-compiled libraries of the dependencies
tools/ contains misc tools 

License
=======

See LICENSE for the current software license governing fg.
