
#include "blog_server.h"
#include "tools/sf_logger.hpp"

using namespace skyfire;
using namespace std;

blog_server::blog_server(const std::string &config_file_path) {
    config__ = config_manager::make_instance(config_file_path);
    if (!config__->inited())
    {
        sf_error("config file load error", config_file_path);
        assert(false);
    }

    auto server_json_config = config__->value("server"s, sf_json());
    if (server_json_config.is_null())
    {
        sf_error("config error");
        assert(false);
    }
    sf_http_server_config server_conf;
    from_json(server_json_config, server_conf);

    server__ = sf_http_server::make_instance(server_conf);

    string static_path = string(config__->value("static_path"s, sf_json(".")));
    server__->add_static_router(static_path, {{"GET"s}});

    server__->add_http_router("/admin", std::function([this](const sf_http_request& req, sf_http_response& res){
        admin_root(req, res);
    }));

    server__->add_http_router("/admin/login", std::function([this](const sf_http_request& req, sf_http_response &res){
        admin_login(req, res);
    }), {{"POST"s}});
}

bool blog_server::start() {
    return server__->start();
}

void blog_server::admin_root(const sf_http_request &req, sf_http_response &res) {
    res.get_header().set_header("Location", "/admin/index.html");
    res.set_status(302);
}

void blog_server::admin_login(const sf_http_request& req, sf_http_response &res) {
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("user") == 0 || param.count("password") == 0)
    {
        res.set_status(403);
    }

    // TODO 验证用户名密码
    if(param["user"] != "skyfire" || param["password"] != "123456")
    {
        res.set_status(403);
    }
    else {
        res.get_header().set_header("Location", "/admin/manage.html");
        res.set_status(302);
    }
}

