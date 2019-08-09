#!/bin/bash
set -e
mkdir -p /opt/my_blog/db
cd /opt/my_blog/
rm -rf build blog_server
mkdir build
cd build
cmake ..
make -j
cd src
cp blog_server /opt/my_blog/
cd /opt/my_blog
./blog_server