#pragma once

#include "blog_config.h"
#include "config_manager.h"
#include "database.h"
#include "inja.hpp"
#include <sf_http_server>
// #include "network/websocket_param_t.hpp"

using namespace skyfire;

class blog_server : public make_instance_t<blog_server, empty_class> {
private:
    std::shared_ptr<http_server> server__;
    std::shared_ptr<config_manager> config__;

    inja::Environment env;

    blog_config blog_config__;

    std::shared_ptr<database> database__ = nullptr;

    static nlohmann::json json_to_nlo_json(const json& js)
    {
        return nlohmann::json::parse(js.to_string());
    }

    static bool check_param(const http_param_t& param,
        const vector<string>& keys)
    {
        for (auto& p : keys) {
            if (!param.contains(p)) {
                return false;
            }
        }
        return true;
    }

    json gen_blog_info(const blog& data);

    void admin_root(const http_server_request& req, http_server_response& res);

    void admin_login(const http_server_request& req, http_server_response& res);

    bool admin_check(const http_server_request& req, http_server_response& res);

    void user_info(const http_server_request& req, http_server_response& res);

    void set_base_info(const http_server_request& req, http_server_response& res);

    void change_password(const http_server_request& req, http_server_response& res);

    void logout(const http_server_request& req, http_server_response& res);

    void setup_server(const http_server_config& server_conf);

    void group_info(const http_server_request& req, http_server_response& res);

    json group_info();

    void add_big_group(const http_server_request& req, http_server_response& res);

    void add_sub_group(const http_server_request& req, http_server_response& res);

    void delete_big_group(const http_server_request& req, http_server_response& res);

    void delete_sub_group(const http_server_request& req, http_server_response& res);

    void rename_big_group(const http_server_request& req, http_server_response& res);

    void rename_sub_group(const http_server_request& req, http_server_response& res);

    void add_label(const http_server_request& req, http_server_response& res);

    void rename_label(const http_server_request& req, http_server_response& res);

    void get_label(const http_server_request& req, http_server_response& res);

    void delete_label(const http_server_request& req, http_server_response& res);

    void blog_list(const http_server_request& req, http_server_response& res);

    void get_blog(const http_server_request& req, http_server_response& res);

    void draft_list(const http_server_request& req, http_server_response& res);

    void delete_draft_list(const http_server_request& req, http_server_response& res);

    void update_draft(const http_server_request& req, http_server_response& res);

    void delete_draft(const http_server_request& req, http_server_response& res);

    void add_blog(const http_server_request& req, http_server_response& res);

    void editor(const http_server_request& req, http_server_response& res);

    void get_draft(const http_server_request& req, http_server_response& res);

    void set_top(const http_server_request& req, http_server_response& res);

    void set_top_bat(const http_server_request& req, http_server_response& res);

    void set_hide_bat(const http_server_request& req, http_server_response& res);

    void delete_blog_bat(const http_server_request& req, http_server_response& res);

    void update_blog_group_bat(const http_server_request& req,
        http_server_response& res);

    void add_blog_label_bat(const http_server_request& req, http_server_response& res);

    void set_hide(const http_server_request& req, http_server_response& res);

    void delete_blog(const http_server_request& req, http_server_response& res);

    void get_blog_content(const http_server_request& req, http_server_response& res);

    void add_blog_label(const http_server_request& req, http_server_response& res);

    void delete_blog_label(const http_server_request& req, http_server_response& res);

    void update_blog_group(const http_server_request& req, http_server_response& res);

    void user_get_blog(const http_server_request& req, http_server_response& res);

    void user_get_all_blog(const http_server_request& req, http_server_response& res);

    void user_get_content(const http_server_request& req, http_server_response& res);

    void user_get_label(const http_server_request& req, http_server_response& res);

    void get_blog_info(const http_server_request& req, http_server_response& res);

    void read_blog(const http_server_request& req, http_server_response& res);

    void uploaded_file_list(const http_server_request& req, http_server_response& res);

    void heart_beat(const http_server_request& req, http_server_response& res);

    void upload_file(const http_server_request& req, http_server_response& res);

    void delete_file(const http_server_request& req, http_server_response& res);

    void add_comment(const http_server_request& req, http_server_response& res);

    void delete_comment(const http_server_request& req, http_server_response& res);

    void audit_comment(const http_server_request& req, http_server_response& res);

    void get_comment(const http_server_request& req, http_server_response& res);

    void user_get_comment(const http_server_request& req, http_server_response& res);

    void get_audited_comment(const http_server_request& req, http_server_response& res);

    void download_md(const http_server_request& req, http_server_response& res, string id);

public:
    explicit blog_server(const std::string& config_file_path);

    bool start();
};
