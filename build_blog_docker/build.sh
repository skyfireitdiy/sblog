#!/bin/bash

rm -rvf build
mkdir build
cd build
cmake ..
make
cd blog_server
cp ../../config.json .
./blog_server