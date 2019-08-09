
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

    // todo 创建router

    auto root_router = sf_http_part_router::make_instance("/"s, [this](const sf_http_request& req, sf_http_response& res){
        return true;
    });
    root_router->add_router(sf_static_router::make_instance(static_path));


    auto admin_router =  sf_http_part_router::make_instance("/admin"s, [this](const sf_http_request& req, sf_http_response& res){
        return admin_check(req, res);
    });
    admin_router->add_router(sf_static_router::make_instance(static_path));

    admin_router->add_router(sf_http_router::make_instance("/"s, function([this](const sf_http_request& req, sf_http_response& res){
        admin_root(req, res);
    })));

    root_router->add_router(admin_router);

    auto api_router = sf_http_part_router::make_instance("/api"s, [this](const sf_http_request& req, sf_http_response& res){
        return true;
    });
    api_router->add_router(sf_http_router::make_instance("/login"s, function([this](const sf_http_request& req, sf_http_response& res){
        admin_login(req, res);
    })));

    root_router->add_router(api_router);

    auto admin_api_router = sf_http_part_router::make_instance("/api"s, [this](const sf_http_request& req, sf_http_response&res){
        return true;
    });

    admin_api_router->add_router(sf_http_router::make_instance("/user_info"s, function([this](const sf_http_request& req, sf_http_response& res){
        get_user_info(req, res);
    }), vector{{"GET"s}}));

    admin_api_router->add_router(sf_http_router::make_instance("/user_info"s, function([this](const sf_http_request& req, sf_http_response& res){
        set_user_info(req, res);
    }), vector{{"POST"s}}));


    admin_router->add_router(admin_api_router);

    server__->add_router(root_router);

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
    if (param.count("name") == 0 || param.count("password") == 0)
    {
        ret["code"] = 1;
        ret["msg"] = "param error";
        ret["redirect"] = "/html/admin_login.html";
        return;
    }

    auto user_info = admin_user_manager__->check_user(param["name"], param["password"]);

    if(user_info == nullptr)
    {
        ret["code"] = 2;
        ret["msg"] = "user name or password error";
        ret["redirect"] = "/html/admin_login.html";
        return;
    }
    else {
        auto session_id = req.get_session_id();
        server__->set_session(session_id, "user", to_json(*user_info));
        ret["code"] = 0;
        ret["msg"] = "ok";
        ret["redirect"] = "/admin/html/manage.html";
    }
}

bool blog_server::admin_check(const sf_http_request &req, sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    if(!session.has("user"))
    {
        res.redirect("/html/admin_login.html");
        return false;
    }
    return true;
}

void blog_server::admin_root(const sf_http_request &req, sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    if(!session.has("user"))
    {
        res.redirect("/html/admin_login.html");
    }
    else{
        res.redirect("/admin/html/manage.html");
    }
}

void blog_server::get_user_info(const sf_http_request &req, sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    sf_json ret;
    ret["code"] = 0;
    ret["data"] = session["user"];
    res.set_json(ret);
}

void blog_server::set_user_info(const sf_http_request &req, sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] {
        res.set_json(ret);
    });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("name") == 0 ||
        param.count("qq") == 0 ||
        param.count("website") == 0 ||
        param.count("desc") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
    } else{
        auto session = server__->get_session(req.get_session_id());
        session["user"]["name"] = param["name"];
        session["user"]["qq"] = param["qq"];
        session["user"]["website"] = param["website"];
        session["user"]["desc"] = param["desc"];
        admin_user user;
        from_json(session["user"], user);
        admin_user_manager__->update_user_info(user);
        ret["code"] = 0;
        ret["data"] = session["user"].clone();
        ret["data"].remove("password");
    }
}

