#include "database.h"
#include "digestpp.hpp"
#include "tools/sf_logger.hpp"

database::database(const string &path) {
    sf_info("database path", path);
    storage__ = make_unique<StorageType>(init_user_storage(path));
    storage__->sync_schema();
}

bool database::check_user_empty() {
    return storage__->count<admin_user>() == 0;
}

shared_ptr<admin_user> database::check_user(const string &name,
                                            const string &password) {
    auto users = storage__->get_all<admin_user>(
        where(c(&admin_user::name) == name and
              c(&admin_user::password) == hash_password(password)));
    if (users.empty()) {
        return nullptr;
    }
    return std::make_shared<admin_user>(users[0]);
}

void database::update_user_info(const admin_user &user) {
    storage__->update(user);
}

string hash_password(const std::string &password) {
    return digestpp::sha512().absorb(password).hexdigest();
}

void database::insert_user(const admin_user &user) { storage__->insert(user); }

vector<blog_big_type> database::get_all_big_type() {
    return storage__->get_all<blog_big_type>();
}

vector<blog_sub_type> database::get_all_sub_type() {
    return storage__->get_all<blog_sub_type>();
}

map<int, int> database::get_sub_type_article_count() {
    map<int, int> ret;
    auto data = storage__->select(columns(&blog::sub_type, count(&blog::id)),
                                  group_by(&blog::sub_type));
    for (auto &p : data) {
        ret[get<0>(p)] = get<1>(p);
    }
    return ret;
}

int database::get_sub_type_article_count(int sub_type) {
    return storage__->count<blog>(where(c(&blog::sub_type) == sub_type));
}

shared_ptr<blog_big_type> database::check_big_type(const string &type_name) {
    auto big_types = storage__->get_all<blog_big_type>(
        where(c(&blog_big_type::type_name) == type_name));
    if (big_types.empty()) {
        return nullptr;
    }
    return make_shared<blog_big_type>(big_types[0]);
}

shared_ptr<blog_sub_type> database::check_sub_type(int big_type,
                                                   const string &type_name) {
    auto sub_types = storage__->get_all<blog_sub_type>(
        where(c(&blog_sub_type::big_type) == big_type and
              c(&blog_sub_type::type_name) == type_name));
    if (sub_types.empty()) {
        return nullptr;
    }
    return make_shared<blog_sub_type>(sub_types[0]);
}

void database::insert_big_type(const blog_big_type &big_type) {
    storage__->insert(big_type);
}

void database::insert_sub_type(const blog_sub_type &sub_type) {
    storage__->insert(sub_type);
}

int database::get_sub_type_count(int big_type) {
    return storage__->count<blog_sub_type>(
        where(c(&blog_sub_type::big_type) == big_type));
}

void database::delete_big_type(int big_type) {
    storage__->remove<blog_big_type>(big_type);
}

void database::delete_sub_type(int sub_type) {
    storage__->remove<blog_sub_type>(sub_type);
}

void database::update_big_type(const blog_big_type &big_type) {
    storage__->update(big_type);
}

void database::update_sub_type(const blog_sub_type &sub_type) {
    storage__->update(sub_type);
}

shared_ptr<blog_big_type> database::get_big_type(int id) {
    return storage__->get_pointer<blog_big_type>(id);
}

shared_ptr<blog_sub_type> database::get_sub_type(int id) {
    return storage__->get_pointer<blog_sub_type>(id);
}

shared_ptr<label> database::check_label(const string &name) {
    auto data = storage__->get_all<label>(where(c(&label::label_name) == name));
    if (data.empty()) {
        return nullptr;
    }
    return make_shared<label>(data[0]);
}

void database::update_label(const label &lab) { storage__->update(lab); }

void database::insert_label(const label &lab) { storage__->insert(lab); }

void database::delete_label(int id) { storage__->remove<label>(id); }

vector<label> database::get_all_label() { return storage__->get_all<label>(); }

shared_ptr<label> database::get_label(int id) {
    return storage__->get_pointer<label>(id);
}

vector<blog> database::get_all_blog() { return storage__->get_all<blog>(); }

shared_ptr<blog_content> database::get_blog_content(int id) {
    return storage__->get_pointer<blog_content>(id);
}

vector<label> database::get_blog_labels(int blog_id) {
    auto data = storage__->select(
        columns(&label::id, &label::label_name),
        join<blog_label>(on(c(&label::id) == &blog_label::label_id and
                            c(&blog_label::blog_id) == blog_id)));
    vector<label> ret(data.size());
    for (auto i = 0UL; i < data.size(); ++i) {
        ret[i] = label{get<0>(data[i]), std::get<1>(data[i])};
    }
    return ret;
}

void database::insert_blog(const blog &b) { storage__->insert(b); }

void database::delete_blog(int blog_id) { storage__->remove<blog>(blog_id); }

void database::update_blog(const blog &b) { storage__->update(b); }

shared_ptr<blog> database::get_blog(int id) {
    return storage__->get_pointer<blog>(id);
}

void database::insert_blog_content(const blog_content &bc) {
    storage__->insert(bc);
}

void database::delete_blog_content(int blog_id) {
    storage__->remove<blog_content>(blog_id);
}

void database::update_blog_content(const blog_content &bc) {
    storage__->update(bc);
}