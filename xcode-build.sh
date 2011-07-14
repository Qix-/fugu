# echo Warning, Xcode project doesnt work yet, try osx-make instead for now..
# exit 

echo Building XCode Project...
mkdir -p build/xcode
# cp lib/osx/*.dylib build/xcode
cd build/xcode
rm CMakeCache.txt
cmake -G "Xcode" ../..
cd ../..
echo Done!

