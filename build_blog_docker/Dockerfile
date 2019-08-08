from wmb521/cpp_build_docker:v1
copy build.sh /tmp/
run mkdir -p /home/skyfire/Desktop/my_blog && \
    chmod +x /tmp/build.sh

workdir /home/skyfire/Desktop/my_blog
cmd ["/tmp/build.sh"]