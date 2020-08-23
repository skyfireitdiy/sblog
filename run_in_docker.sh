#!/bin/bash
docker run --rm -v$(pwd):/opt/my_blog -it --name my_blog -p 8080:8080 blog_build_docker
