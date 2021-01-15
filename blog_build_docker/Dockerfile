FROM alpine
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories
RUN apk add --no-cache curl bash clang
RUN apk add --no-cache sqlite-dev
RUN apk add --no-cache openssl-dev
RUN apk add --no-cache zlib-dev
RUN curl -fsSL https://xmake.io/shget.text | bash
RUN rm -rf /var/cache/apk/* 
RUN rm -rf /root/.cache 
RUN rm -rf /tmp/*
ENV PATH /root/.local/bin:$PATH