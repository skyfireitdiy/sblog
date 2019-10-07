# 个人博客

## 编译运行

因为项目中代码使用的编译器版本较高，所以搭建了docker编译运行环境。可根据以下步骤编译运行：

1. 环境准备

* 安装 docker 与 docker-compose
* 克隆代码
```bash
git clone https://github.com/skyfireitdiy/my_blog --recursive
```

2. 构建docker镜像

```bash
./make_docker.sh
```

**注意：如果当前用户没有加入docker用户组，需要使用root权限运行，下同**

3. 重命名配置文件

```bash
mv config.json.example config.json                      # server配置
mv nginx.conf.example nginx.conf                        # nginx配置
mv docker-compose.yml.example docker-compose.yml        # docker-compose 配置
```

4. 编译运行

```bash
./run_in_docker.sh
```

浏览器访问[http://localhost:8080/admin](http://localhost:8080/admin)进入后台管理页面

访问[http://localhost:8080](http://localhost:8080)进入首页


5. 停止

```bash
./stop_docker.sh
```