FROM blog_build_docker
RUN mkdir -p /my_blog
RUN mkdir -p /my_blog/db
RUN mkdir -p /my_blog/file
COPY blog_server /my_blog/
COPY static /my_blog/static
COPY template /my_blog/template
COPY config.json /my_blog/config.json
RUN chmod +x /my_blog/blog_server
WORKDIR /my_blog
ENTRYPOINT ["./blog_server"]
