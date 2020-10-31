from codehz/xmake
copy build.sh /tmp/
copy run.sh /tmp/
run chmod +x /tmp/build.sh /tmp/run.sh
workdir /opt/my_blog
run sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && apk update
run apk add sqlite-dev
run apk add openssl-dev
run apk add zlib-dev
cmd ["/tmp/build.sh"]