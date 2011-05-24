
echo Building Makefile Project...
mkdir -p build/osx-make
cp lib/osx/*.dylib build/osx-make
cd build/osx-make
rm CMakeCache.txt
cmake -G "Unix Makefiles" ../../src/misc/iso
cd ../..
echo Done!

