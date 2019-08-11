#pragma once

#include "admin_user.h"
#include "blog_config.h"
#include "blog_manager.h"
#include "config_manager.h"
#include "network/sf_http_server.hpp"

using namespace skyfire;

class blog_server : public sf_make_instance_t<blog_server, sf_empty_class> {
private:
  std::shared_ptr<sf_http_server> server__;
  std::shared_ptr<config_manager> config__;

  blog_config blog_config__;

  std::shared_ptr<admin_user_manager> admin_user_manager__ = nullptr;
  std::shared_ptr<blog_manager> blog_manager__ = nullptr;

  void admin_root(const sf_http_request &req, sf_http_response &res);

  void admin_login(const sf_http_request &req, sf_http_response &res);

  bool admin_check(const sf_http_request &req, sf_http_response &res);

  void get_user_info(const sf_http_request &req, sf_http_response &res);

  void set_user_info(const sf_http_request &req, sf_http_response &res);

  void change_password(const sf_http_request &req, sf_http_response &res);

  void logout(const sf_http_request &req, sf_http_response &res);

  void setup_server(const sf_http_server_config &server_conf);

public:
  explicit blog_server(const std::string &config_file_path);

  bool start();
};
