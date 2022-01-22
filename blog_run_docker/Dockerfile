FROM blog_build_docker
RUN mkdir -p /sblog
RUN mkdir -p /sblog/db
RUN mkdir -p /sblog/file
COPY blog_server /sblog/
COPY static /sblog/static
COPY template /sblog/template
COPY config.json /sblog/config.json
RUN chmod +x /sblog/blog_server
WORKDIR /sblog
ENTRYPOINT ["./blog_server"]
