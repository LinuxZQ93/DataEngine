#!/bin/sh
if [ ! -d Build ];then
    mkdir Build
fi
dir=x86
cd ./Build
if [ ! -n "$1" ];then
cmake .. -DCMAKE_BUILD_TYPE=Debug -Dplatform=x86
else
cmake .. -DCMAKE_BUILD_TYPE=Debug -Dplatform=$1
dir=$1
fi
make -j8

cd -
rm -rf ./Run/Lib/*
cp ./Lib/ThirdLib/$dir/* ./Run/Lib
cp ./Run/$dir/lib* ./Run/Lib
cp ./Run/$dir/Main ./Run/
cp ./out/$dir/Debug/* ./Run/Lib
