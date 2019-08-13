
#include "blog_server.h"
#include "blog_config.h"
#include "database.h"
#include "network/sf_http_part_router.hpp"
#include "network/sf_websocket_router.hpp"
#include "tools/sf_finally.hpp"
#include "tools/sf_logger.hpp"

using namespace skyfire;
using namespace std;

blog_server::blog_server(const std::string &config_file_path) {
    config__ = config_manager::make_instance(config_file_path);
    if (!config__->inited()) {
        sf_error("config file load error", config_file_path);
        assert(false);
    }

    auto server_json_config = config__->value("server"s);

    if (server_json_config.is_null()) {
        sf_error("server config error");
        assert(false);
    }

    auto blog_json_config = config__->value("blog"s);
    if (blog_json_config.is_null()) {
        sf_error("blog config error");
        assert(false);
    }

    from_json(blog_json_config, blog_config__);

    database__ = database::get_instance(
        sf_path_join(blog_config__.db_path, "database.db3"s));
    if (database__->check_user_empty()) {
        auto default_user_json = config__->value("default_user"s);
        if (default_user_json.is_null()) {
            sf_error("default_user config error");
            assert(false);
        }
        sf_info("user empty, create default user:", default_user_json);
        admin_user user;
        from_json(default_user_json, user);
        user.id = -1;
        user.password = hash_password(user.password);
        database__->insert_user(user);
    }

    sf_http_server_config server_conf;
    from_json(server_json_config, server_conf);

    if (server_conf.port == 0) {
        server_conf.port = 80;
    }
    if (server_conf.host.empty()) {
        server_conf.host = "0.0.0.0";
    }
    if (server_conf.session_timeout == 0) {
        server_conf.session_timeout = 3600;
    }
    if (server_conf.tmp_file_path.empty()) {
        server_conf.tmp_file_path = "/tmp";
    }

    setup_server(server_conf);
}

void blog_server::setup_server(const sf_http_server_config &server_conf) {
    server__ = sf_http_server::make_instance(server_conf);

    auto root_router = sf_http_part_router::make_instance(
        "/"s, [this](const sf_http_request &req, sf_http_response &res) {
            return true;
        });
    root_router->add_router(
        sf_static_router::make_instance(blog_config__.static_path));
    root_router->add_router(sf_http_router::make_instance(
        "/"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            res.redirect("/html/index.html");
        })));

    auto admin_router = sf_http_part_router::make_instance(
        "/admin"s, [this](const sf_http_request &req, sf_http_response &res) {
            return admin_check(req, res);
        });
    admin_router->add_router(
        sf_static_router::make_instance(blog_config__.static_path));

    admin_router->add_router(sf_http_router::make_instance(
        "/"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            admin_root(req, res);
        })));

    root_router->add_router(admin_router);

    auto api_router = sf_http_part_router::make_instance(
        "/api"s, [this](const sf_http_request &req, sf_http_response &res) {
            return true;
        });
    api_router->add_router(sf_http_router::make_instance(
        "/login"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            admin_login(req, res);
        })));

    root_router->add_router(api_router);

    auto admin_api_router = sf_http_part_router::make_instance(
        "/api"s, [this](const sf_http_request &req, sf_http_response &res) {
            return true;
        });

    admin_api_router->add_router(sf_http_router::make_instance(
        "/user_info"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            get_user_info(req, res);
        }),
        vector{{"GET"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/user_info"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            set_user_info(req, res);
        }),
        vector{{"POST"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/change_password"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            change_password(req, res);
        }),
        vector{{"POST"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/logout"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            logout(req, res);
        }),
        vector{{"*"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/group_info"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            get_group_info(req, res);
        }),
        vector{{"GET"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/big_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            add_big_group(req, res);
        }),
        vector{{"POST"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/sub_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            add_sub_group(req, res);
        }),
        vector{{"POST"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/big_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            rename_big_group(req, res);
        }),
        vector{{"PUT"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/sub_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            rename_sub_type(req, res);
        }),
        vector{{"PUT"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/big_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            delete_big_type(req, res);
        }),
        vector{{"DELETE"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/sub_group"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            delete_sub_type(req, res);
        }),
        vector{{"DELETE"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/label"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            get_label(req, res);
        }),
        vector{{"GET"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/label"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            delete_label(req, res);
        }),
        vector{{"DELETE"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/label"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            rename_label(req, res);
        }),
        vector{{"PUT"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/label"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            add_label(req, res);
        }),
        vector{{"POST"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/blog_info"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            get_blog_info(req, res);
        }),
        vector{{"GET"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/draft"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            update_draft(req, res);
        }),
        vector{{"PUT"s}}));

    admin_api_router->add_router(sf_http_router::make_instance(
        "/draft_list"s,
        function([this](const sf_http_request &req, sf_http_response &res) {
            get_draft_list(req, res);
        }),
        vector{{"GET"s}}));

    admin_router->add_router(admin_api_router);

    server__->add_router(root_router);
}

bool blog_server::start() { return server__->start(); }

void blog_server::admin_login(const sf_http_request &req,
                              sf_http_response &res) {
    sf_json ret;
    sf_finally f([&ret, &res] { res.set_json(ret); });

    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("name") == 0 || param.count("password") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        ret["redirect"] = "/html/admin_login.html";
        return;
    }

    auto user_info = database__->check_user(param["name"], param["password"]);

    if (user_info == nullptr) {
        ret["code"] = 2;
        ret["msg"] = "user name or password error";
        ret["redirect"] = "/html/admin_login.html";
        return;
    } else {
        auto session_id = req.get_session_id();
        server__->set_session(session_id, "user", to_json(*user_info));
        ret["code"] = 0;
        ret["msg"] = "ok";
        ret["redirect"] = "/admin/html/manage.html";
    }
}

bool blog_server::admin_check(const sf_http_request &req,
                              sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    if (!session.has("user")) {
        res.redirect("/html/admin_login.html");
        return false;
    }
    return true;
}

void blog_server::admin_root(const sf_http_request &req,
                             sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    if (!session.has("user")) {
        res.redirect("/html/admin_login.html");
    } else {
        res.redirect("/admin/html/manage.html");
    }
}

void blog_server::get_user_info(const sf_http_request &req,
                                sf_http_response &res) {
    auto session_id = req.get_session_id();
    auto session = server__->get_session(session_id);
    sf_json ret;
    ret["code"] = 0;
    ret["data"] = session["user"].clone();
    ret["data"].remove("password");
    res.set_json(ret);
}

void blog_server::set_user_info(const sf_http_request &req,
                                sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("name") == 0 || param.count("qq") == 0 ||
        param.count("website") == 0 || param.count("desc") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
    } else {
        auto session = server__->get_session(req.get_session_id());
        session["user"]["name"] = param["name"];
        session["user"]["qq"] = param["qq"];
        session["user"]["website"] = param["website"];
        session["user"]["desc"] = param["desc"];
        admin_user user;
        from_json(session["user"], user);
        database__->update_user_info(user);
        ret["code"] = 0;
    }
}

void blog_server::change_password(const sf_http_request &req,
                                  sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("old_password") == 0 || param.count("new_password") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
    } else {
        auto session = server__->get_session(req.get_session_id());
        if (static_cast<string>(session["user"]["password"]) ==
            hash_password(param["old_password"])) {
            session["user"]["password"] = hash_password(param["new_password"]);
            admin_user user;
            from_json(session["user"], user);
            database__->update_user_info(user);
            ret["code"] = 0;
        } else {
            ret["code"] = 2;
            ret["msg"] = "old password error";
        }
    }
}

void blog_server::logout(const sf_http_request &req, sf_http_response &res) {
    auto session = server__->get_session(req.get_session_id());
    session.remove("user");
    res.redirect("/html/admin_login.html");
}

void blog_server::get_group_info(const sf_http_request &req,
                                 sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto big_types = database__->get_all_big_type();
    auto sub_types = database__->get_all_sub_type();
    auto article_count = database__->get_sub_type_article_count();
    ret["code"] = 0;
    ret["data"] = sf_json();
    for (auto &p : big_types) {
        sf_json tmp = to_json(p);
        tmp["sub_type"] = sf_json();
        tmp["sub_type"].convert_to_array();
        for (auto &q : sub_types) {
            if (q.big_type == p.id) {
                sf_json tmp_sub = to_json(q);
                if (article_count.count(q.id) != 0) {
                    tmp_sub["article_count"] = article_count[q.id];
                } else {
                    tmp_sub["article_count"] = 0;
                }
                tmp["sub_type"].append(tmp_sub);
            }
        }
        ret["data"].append(tmp);
    }
}

void blog_server::add_big_group(const sf_http_request &req,
                                sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("type_name") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    };
    if (database__->check_big_type(param["type_name"]) != nullptr) {
        ret["code"] = 2;
        ret["msg"] = "type name already exists";
        return;
    }

    blog_big_type big_type{-1, param["type_name"]};

    database__->insert_big_type(big_type);
    ret["code"] = 0;
}

void blog_server::add_sub_group(const sf_http_request &req,
                                sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("big_type") == 0 || param.count("type_name") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto big_type =
        static_cast<int>(sf_string_to_long_double(param["big_type"]));
    if (database__->check_sub_type(big_type, param["type_name"]) != nullptr) {
        ret["code"] = 1;
        ret["msg"] = "type name already exists";
        return;
    }
    blog_sub_type sub_type{-1, big_type, param["type_name"]};
    database__->insert_sub_type(sub_type);
    ret["code"] = 0;
}

void blog_server::delete_big_type(const sf_http_request &req,
                                  sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto big_type = static_cast<int>(sf_string_to_long_double(param["id"]));
    if (database__->get_sub_type_count(big_type) != 0) {
        ret["code"] = 2;
        ret["msg"] = "big type has sub type";
        return;
    }
    database__->delete_big_type(big_type);
    ret["code"] = 0;
}

void blog_server::delete_sub_type(const sf_http_request &req,
                                  sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto sub_type = static_cast<int>(sf_string_to_long_double(param["id"]));
    if (database__->get_sub_type_article_count(sub_type) != 0) {
        ret["code"] = 2;
        ret["msg"] = "sub type has articles";
        return;
    }
    database__->delete_sub_type(sub_type);
    ret["code"] = 0;
}

void blog_server::rename_big_group(const sf_http_request &req,
                                   sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0 || param.count("new_name") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto big_type = static_cast<int>(sf_string_to_long_double(param["id"]));
    if (database__->get_big_type(big_type) == nullptr) {
        ret["code"] = 2;
        ret["msg"] = "type id not found";
        return;
    }
    if (database__->check_big_type(param["new_name"]) != nullptr) {
        ret["code"] = 3;
        ret["msg"] = "type name already exists";
        return;
    }
    blog_big_type t{big_type, param["new_name"]};
    database__->update_big_type(t);
    ret["code"] = 0;
}

void blog_server::rename_sub_type(const sf_http_request &req,
                                  sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0 || param.count("new_name") == 0 ||
        param.count("big_type") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto big_type =
        static_cast<int>(sf_string_to_long_double(param["big_type"]));
    auto sub_type = static_cast<int>(sf_string_to_long_double(param["id"]));

    if (database__->get_sub_type(sub_type) == nullptr) {
        ret["code"] = 2;
        ret["msg"] = "type id not found";
        return;
    }
    if (database__->check_sub_type(big_type, param["new_name"]) != nullptr) {
        ret["code"] = 3;
        ret["msg"] = "type name already exists";
        return;
    }
    blog_sub_type t{sub_type, big_type, param["new_name"]};
    database__->update_sub_type(t);
    ret["code"] = 0;
}

void blog_server::get_label(const sf_http_request &req, sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto data = database__->get_all_label();
    ret["code"] = 0;
    ret["data"] = to_json(data);
}

void blog_server::add_label(const sf_http_request &req, sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("name") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    if (database__->check_label(param["name"]) != nullptr) {
        ret["code"] = 2;
        ret["msg"] = "label already exists";
        return;
    }
    label lab{-1, param["name"]};
    database__->insert_label(lab);
    ret["code"] = 0;
}

void blog_server::rename_label(const sf_http_request &req,
                               sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0 || param.count("name") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto lab_id = static_cast<int>(sf_string_to_long_double(param["id"]));
    if (database__->get_label(lab_id) == nullptr) {
        ret["code"] = 2;
        ret["msg"] = "label not found";
        return;
    }
    if (database__->check_label(param["name"]) != nullptr) {
        ret["code"] = 3;
        ret["msg"] = "label already exists";
        return;
    }
    label lab{lab_id, param["name"]};
    database__->update_label(lab);
    ret["code"] = 0;
}

void blog_server::delete_label(const sf_http_request &req,
                               sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param name";
        return;
    }
    auto lab_id = static_cast<int>(sf_string_to_long_double(param["id"]));
    if (database__->get_label(lab_id) == nullptr) {
        ret["code"] = 2;
        ret["msg"] = "label not found";
        return;
    }
    database__->delete_label(lab_id);
    ret["code"] = 0;
}

void blog_server::get_blog_info(const sf_http_request &req,
                                sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto blogs = database__->get_all_blog();
    auto blogs_json = to_json(blogs);
    for (int i = 0; i < blogs.size(); ++i) {
        auto sub_type = database__->get_sub_type(blogs[i].sub_type);
        if (sub_type == nullptr) {
            ret["code"] = 1;
            ret["msg"] = "server error";
            return;
        }
        blogs_json[i]["sub_type"] = to_json(*sub_type);
        auto big_type = database__->get_big_type(sub_type->big_type);
        if (big_type == nullptr) {
            ret["code"] = 2;
            ret["msg"] = "server error";
            return;
        }
        blogs_json[i]["big_type"] = to_json(*big_type);
        auto labels = database__->get_blog_labels(blogs[i].id);
        blogs_json[i]["labels"] = to_json(labels);
    }
    ret["code"] = 0;
    ret["data"] = blogs_json;
}

void blog_server::get_draft_list(const sf_http_request &req,
                                 sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    ret["code"] = 0;
    ret["data"] = to_json(database__->get_all_draft());
}

void blog_server::update_draft(const sf_http_request &req,
                               sf_http_response &res) {
    sf_json ret;
    sf_finally f([&res, &ret] { res.set_json(ret); });
    auto param = sf_parse_param(to_string(req.get_body()));
    if (param.count("id") == 0 || param.count("title") == 0 ||
        param.count("content") == 0) {
        ret["code"] = 1;
        ret["msg"] = "param error";
        return;
    }
    auto draft_id = static_cast<int>(sf_string_to_long_double(param["id"]));
    draft df{draft_id, param["title"], param["content"]};
    ret["code"] = 0;
    if (df.id == -1) {
        ret["data"] = database__->insert_draft(df);
    } else {
        ret["data"] = df.id;
    }
}