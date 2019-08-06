#pragma once

#include "network/sf_http_server.hpp"
#include "config_manager.h"
#include "admin_user.h"


using namespace skyfire;

class blog_server: public sf_make_instance_t<blog_server, sf_empty_class> {
private:
    std::shared_ptr<sf_http_server> server__;
    std::shared_ptr<config_manager> config__;

    std::shared_ptr<admin_user_manager> admin_user_manager__ = nullptr;

    void admin_root(const sf_http_request& req, sf_http_response &res);

    void admin_login(const sf_http_request& req, sf_http_response &res);

public:
    explicit blog_server(const std::string& config_file_path);

    bool start();
};


