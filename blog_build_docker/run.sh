#!/bin/bash
cd /opt/my_blog
ulimit -c unlimited
while [ 1 ]
do
    ./blog_server
done
