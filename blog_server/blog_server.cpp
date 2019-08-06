
#include "blog_server.h"
#include "tools/sf_logger.hpp"
#include "tools/sf_finally.hpp"

using namespace skyfire;
using namespace std;

blog_server::blog_server(const std::string &config_file_path) {
    config__ = config_manager::make_instance(config_file_path);
    if (!config__->inited())
    {
        sf_error("config file load error", config_file_path);
        assert(false);
    }

    auto server_json_config = config__->value("server"s);
    if (server_json_config.is_null())
    {
        sf_error("server config error");
        assert(false);
    }

    auto db_path = config__->value("db_path"s);
    if(db_path.is_null())
    {
        sf_error("db_path config error");
        assert(false);
    }

    admin_user_manager__ = admin_user_manager::get_instance(sf_path_join(string(db_path), "admin_user.db3"s));

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
    res.redirect("/admin/index.html");
}

void blog_server::admin_login(const sf_http_request& req, sf_http_response &res) {
    sf_json ret;
    sf_finally f([&ret, &res]{
        res.set_body(to_byte_array(ret.to_string()));
    });

    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("user") == 0 || param.count("password") == 0)
    {
        ret["code"] = 1;
        ret["msg"] = "param error";
        ret["redirect"] = "/admin/index.html";
        return;
    }

    auto user_info = admin_user_manager__->check_user(param["user"], param["password"]);

    if(user_info == nullptr)
    {
        ret["code"] = 2;
        ret["msg"] = "user name or password error";
        ret["redirect"] = "/admin/index.html";
        return;
    }
    else {
        auto session_id = req.get_session_id();
        server__->set_session(session_id, "user", to_json(*user_info));
        ret["code"] = 0;
        ret["msg"] = "ok";
        ret["redirect"] = "/admin/manage.html";
    }
}

