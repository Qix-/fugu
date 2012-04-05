Fugu/fg is a system for procedurally generating animated geometric forms in real-time. Fugu can be downloaded from http://bp.io/fugu, an online reference and gallery is also located there.

This readme is for people who want to compile and hack the source of fugu. Please see LICENSE.txt for the license governing use of the fg source, and CREDITS.txt for a list of programmers on the project and list of libraries used.

Directory Structure
===================
src/ contains the source for fugu and the cmake build script
build-*/ will contain the build files after running one of the build batch files
include/ contains headers for dependencies
scripts/ contains the fg lua scripts (scripts/ex contains the latest working examples)
core/ contains the fg lua core functions and documentation
lib/ contains pre-compiled libraries of the dependencies
tools/ contains misc tools
doc/ contains both the doxygen file for c++ comments and the template for the online reference
site/ contains the source of the website at http://bp.io/fugu

Building Instructions
=====================
Fugu is built using Cmake 2.8. As such, it should be possible to output project files for Xcode, Visual Studio, etc. Some build scripts are provided for some of these, to generate other projects you will need to write your own scripts. Some example build sequences (from within the fg/ directory):

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

Linux/Ubuntu/Debian
-------------------
> ./linux-make-build.sh
> cd build-linux-make
> make install

Fugu uses Qt extensively, so you will need to have that installed (version 4.7.4 or later). For the osx/make and windows/mingw targets, most of the dependencies have been prebuilt and reside in lib/. If you target other systems, e.g., Visual Studio, then you may need to build the dependencies for that target. 

For Linux (on Debian), you'll need qt4, lua5.1, libglfw2, libglew, libluabind, boost, libboost-system-dev, libboost-filesystem, libqscintilla2-dev

I'm sure there's something I've left out, so please let me know if you have any trouble,
Ben Porter (01/2012)
