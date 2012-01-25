
echo Building Makefile Project...
mkdir -p build-osx-make
cd build-osx-make
rm CMakeCache.txt
cmake -G "Unix Makefiles" ../src
cd ..
echo Done!

