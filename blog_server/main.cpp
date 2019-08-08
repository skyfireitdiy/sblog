#include "blog_server.h"

int main()
{
    auto server = blog_server::make_instance("config.json");
    server->start();
}