//
// Created by skyfire on 19-8-4.
//

#include "base_server.h"
#include "network/sf_http_server.hpp"

using namespace skyfire;

int main()
{
    sf_http_server_config config;
    config.host = "0.0.0.0";
    config.port = 8080;
    config.tmp_file_path = "/tmp";
    auto server = sf_http_server::make_instance(config);

    server->add_http_router("/", std::function([](const sf_http_request& req, sf_http_response& res){
        res.set_status(302);
        res.get_header().set_header("Location", "/index.html");
    }), {{"GET"s}});

    server->add_static_router("../../admin/static", {{"GET"s}});

    server->start();
}