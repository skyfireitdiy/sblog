from codehz/xmake
copy build.sh /tmp/
run chmod +x /tmp/build.sh
workdir /opt/my_blog
run sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && apk update
run apk add sqlite-dev
run apk add openssl-dev
run apk add zlib-dev
cmd ["/tmp/build.sh"]