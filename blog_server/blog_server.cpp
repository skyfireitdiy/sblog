
#include "blog_server.h"
#include "tools/sf_logger.hpp"
#include "tools/sf_finally.hpp"
#include "network/sf_http_part_router.hpp"

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

    if(server_conf.port == 0){
        server_conf.port = 80;
    }
    if(server_conf.host.empty())
    {
        server_conf.host = "0.0.0.0";
    }
    if(server_conf.session_timeout == 0)
    {
        server_conf.session_timeout = 3600;
    }
    if(server_conf.tmp_file_path.empty())
    {
        server_conf.tmp_file_path = "/tmp";
    }

    server__ = sf_http_server::make_instance(server_conf);

    string static_path = string(config__->value("static_path"s, sf_json(".")));

    auto admin_part = sf_http_part_router::make_instance("/admin"s, function([this](const sf_http_request& req, sf_http_response& res){
        return admin(req, res);
    }), vector{{"*"s}}, 0);

    admin_part->add_http_router(sf_http_router::make_instance("/login"s, std::function([this](const sf_http_request& req, sf_http_response &res){
        admin_login(req, res);
    }), vector{{"POST"s}}));

    admin_part->add_http_router(make_static_router(sf_path_join(static_path, "admin"s), {{"GET"s}}));

    server__->add_router(make_static_router(sf_path_join(static_path, "third_part"s), {{"GET"s}}));

    server__->add_router(admin_part);
}

bool blog_server::start() {
    return server__->start();
}

void blog_server::admin_login(const sf_http_request& req, sf_http_response &res) {
    sf_json ret;
    sf_finally f([&ret, &res]{
        res.set_json(ret);
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

bool blog_server::admin(const sf_http_request &req, sf_http_response &res) {
    if(req.get_request_line().url == "/admin/index.html" || req.get_request_line().url == "/admin/login")
    {
        return true;
    }
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    if(!session.has("user"))
    {
        res.redirect("/admin/index.html");
        return false;
    }
    return true;
}

