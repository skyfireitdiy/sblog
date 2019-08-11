#pragma once

#include <string>
#include "sqlite_orm.h"
#include "tools/sf_json.hpp"
#include "tools/sf_single_instance.hpp"

using namespace std;
using namespace skyfire;
using namespace sqlite_orm;

struct admin_user {
    int id;
    string name;
    string password;
    string qq;
    string website;
    string desc;
};

struct blog {
    int id;
    string title;
    string content;
    string publish_time;
    int watch_number;
    int top;
    int sub_type;
};

struct blog_big_type {
    int id;
    string type_name;
};

struct blog_sub_type {
    int id;
    int big_type;
    string type_name;
};

struct label {
    int id;
    string label_name;
};

struct blog_label {
    int blog_id;
    int label_id;
};

SF_JSONIFY(admin_user, id, name, password, qq, website, desc)
SF_JSONIFY(blog, id, title, content, publish_time, watch_number, top, sub_type)
SF_JSONIFY(blog_big_type, id, type_name)
SF_JSONIFY(blog_sub_type, id, big_type, type_name)
SF_JSONIFY(label, id, label_name)
SF_JSONIFY(blog_label, blog_id, label_id)

inline auto init_user_storage(const string &path) {
    return make_storage(
        path,
        make_table("admin_user",
                   make_column("id", &admin_user::id, autoincrement(),
                               primary_key(), unique()),
                   make_column("name", &admin_user::name, unique()),
                   make_column("password", &admin_user::password),
                   make_column("qq", &admin_user::qq),
                   make_column("website", &admin_user::website),
                   make_column("desc", &admin_user::desc)),
        make_table("blog",
                   make_column("id", &blog::id, autoincrement(), primary_key(),
                               unique()),
                   make_column("title", &blog::title),
                   make_column("content", &blog::content),
                   make_column("publish_time", &blog::publish_time),
                   make_column("watch_number", &blog::watch_number),
                   make_column("top", &blog::top),
                   make_column("sub_type", &blog::sub_type)),
        make_table("blog_big_type",
                   make_column("id", &blog_big_type::id, autoincrement(),
                               primary_key(), unique()),
                   make_column("type_name", &blog_big_type::type_name)),
        make_table("blog_sub_type",
                   make_column("id", &blog_sub_type::id, autoincrement(),
                               primary_key(), unique()),
                   make_column("big_type", &blog_sub_type::big_type),
                   make_column("type_name", &blog_sub_type::type_name)),
        make_table("label",
                   make_column("id", &label::id, autoincrement(), primary_key(),
                               unique()),
                   make_column("label_name", &label::label_name)),
        make_table("blog_label", make_column("blog_id", &blog_label::blog_id),
                   make_column("label_id", &blog_label::label_id)));
}

string hash_password(const std::string &password);

class database : public sf_single_instance<database> {
   private:
    using StorageType = decltype(init_user_storage(""));
    std::unique_ptr<StorageType> storage__;

   public:
    bool check_user_empty();

    shared_ptr<admin_user> check_user(const string &name,
                                      const string &password);

    shared_ptr<blog_big_type> check_big_type(const string &type_name);

    shared_ptr<blog_sub_type> check_sub_type(int ig_id,
                                             const string &type_name);

    int get_sub_type_count(int big_type);

    void delete_big_type(int big_type);

    void delete_sub_type(int sub_type);

    void update_user_info(const admin_user &user);

    void insert_user(const admin_user &user);

    vector<blog_big_type> get_all_big_type();

    vector<blog_sub_type> get_all_sub_type();

    map<int, int> get_sub_type_article_count();

    int get_sub_type_article_count(int sub_type);

    void insert_big_type(const blog_big_type &big_type);

    void insert_sub_type(const blog_sub_type &sub_type);

    void update_big_type(const blog_big_type &big_type);

    void update_sub_type(const blog_sub_type &sub_type);

    shared_ptr<blog_big_type> get_big_type(int id);

    shared_ptr<blog_sub_type> get_sub_type(int id);

    explicit database(const string &path);
};
