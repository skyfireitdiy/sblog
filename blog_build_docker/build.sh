#!/bin/bash
set -e
mkdir -p /opt/my_blog/db
mkdir -p /opt/my_blog/file
cd /opt/my_blog/
rm -rf build .xmake blog_server
source ~/.xmake/profile
xmake
cp build/linux/x86_64/release/blog_server /opt/my_blog/
cd /opt/my_blog
ulimit -c unlimited
while [ 1 ]
do
    ./blog_server
done
