#! /bin/bash

cd "$(dirname "$0")"

InstallPrefix="$(pwd)/native-dependencies/output"

mkdir -p "${InstallPrefix}/include/" "${InstallPrefix}/lib/"

if [ ! -e "native-dependencies/zlib-bins/ALL_BUILD.vcxproj" ]; then
  mkdir -p "native-dependencies/zlib-bins"
  cmake "-DCMAKE_INSTALL_PREFIX=$InstallPrefix" -DCMAKE_C_FLAGS_INIT="-m32" -DCMAKE_CXX_FLAGS_INIT="-m32" -S native-dependencies/zlib-1.2.12 -B native-dependencies/zlib-bins
fi

cmake --build native-dependencies/zlib-bins -j 
cmake --install native-dependencies/zlib-bins 
cp native-dependencies/zlib-bins/zconf.h native-dependencies/zlib-1.2.12/zlib.h native-dependencies/output/include/
cp native-dependencies/zlib-bins/libz.a native-dependencies/output/lib/

if [ ! -e "native-dependencies/protobuf-bins/libprotobuf.vcxproj" ]; then
  mkdir -p "native-dependencies/protobuf-bins"
  cmake "-DCMAKE_INSTALL_PREFIX=$InstallPrefix" -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_C_FLAGS_INIT="-m32" -DCMAKE_CXX_FLAGS_INIT="-m32" -S native-dependencies/protobuf-3.15.6/cmake -B native-dependencies/protobuf-bins
fi

cmake --build native-dependencies/protobuf-bins -j 4
cmake --install native-dependencies/protobuf-bins

