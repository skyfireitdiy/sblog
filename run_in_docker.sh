#!/bin/bash
docker run --rm -v$(pwd):/opt/my_blog -it --name my_blog blog_build_docker
docker run --rm -v$(pwd):/opt/my_blog -it --name my_blog_run -p 8080:8080 blog_build_docker /tmp/run.sh
