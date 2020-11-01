#!/bin/bash
docker run --rm -v$(pwd):/opt/my_blog -it --name my_blog blog_build_docker
