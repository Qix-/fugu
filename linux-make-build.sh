
echo Building Makefile Project...
mkdir -p build-linux-make
cd build-linux-make
rm CMakeCache.txt
cmake -G "Unix Makefiles" ../src
cd ..
echo Done!

