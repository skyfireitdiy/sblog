
#include "blog_server.h"
#include <sf_logger>
#include <sf_utils>

using namespace skyfire;

int main() {
    g_logger->add_level_file(SF_INFO_LEVEL, "log/" + make_time_str() + ".log");
    auto server = blog_server::make_instance("config.json");
    server->start();
}