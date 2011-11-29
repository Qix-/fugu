# echo Warning, Xcode project doesnt work yet, try osx-make instead for now..
# exit 

echo Building XCode Project...
mkdir -p build-xcode
cd build-xcode
rm CMakeCache.txt
cmake -G "Xcode" ../src
cd ..
echo Done!

