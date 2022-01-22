
#include "blog_server.h"

using namespace skyfire;

int main()
{
    auto server = blog_server::make_instance("config.json");
    server->start();
}