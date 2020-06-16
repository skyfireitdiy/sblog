#!/bin/bash
docker rm -f my_blog
docker run -v$(pwd):/opt/my_blog -it --name my_blog -p 8080:8080 blog_build_docker
