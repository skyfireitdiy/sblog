#include "blog_server.h"

int main()
{
    auto server = blog_server::make_instance("/home/skyfire/Desktop/my_blog/config.json");
    server->start();
}