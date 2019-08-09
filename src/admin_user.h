#pragma once

#include <string>
#include "sqlite_orm.h"
#include "tools/sf_single_instance.hpp"
#include "tools/sf_json.hpp"


using namespace std;
using namespace skyfire;
using namespace sqlite_orm;

struct admin_user{
    int id;
    string name;
    string password;
    string qq;
    string website;
    string desc;
};

SF_JSONIFY(admin_user, id, name, password, qq, website, desc)

inline auto init_user_storage(const string& path) {
    return make_storage(
            path,
            make_table("admin_user",
                       make_column("id", &admin_user::id, primary_key(), unique()),
                       make_column("name", &admin_user::name, unique()),
                       make_column("password", &admin_user::password),
                       make_column("qq", &admin_user::qq),
                       make_column("website", &admin_user::website),
                       make_column("desc", &admin_user::desc)
            )
    );
}

string hash_password(const std::string& password);


class admin_user_manager : public sf_single_instance<admin_user_manager>{
private:
    using StorageType = decltype(init_user_storage(""));
    std::unique_ptr<StorageType> storage__;
public:

    bool check_empty();

    shared_ptr<admin_user> check_user(const string& name, const string& password);

    explicit admin_user_manager(const string& path);
};
