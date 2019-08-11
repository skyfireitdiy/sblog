#pragma once
#include "sqlite_orm.h"
#include "tools/sf_json.hpp"
#include "tools/sf_utils.hpp"
#include <string>

using namespace std;
using namespace skyfire;
using namespace sqlite_orm;

struct blog {
  int id;
  string title;
  string content;
  string datetime;
  int watch_number;
  int top;
};

SF_JSONIFY(blog, id, title, content, datetime, watch_number, top)

inline auto init_blog_storage(const string &path) {
  return make_storage(
      path, make_table("blog",
                       make_column("id", &blog::id, primary_key(), unique(),
                                   autoincrement()),
                       make_column("title", &blog::title),
                       make_column("content", &blog::content),
                       make_column("datetime", &blog::datetime),
                       make_column("watch_number", &blog::watch_number),
                       make_column("top", &blog::top)));
}

class blog_manager : public sf_single_instance<blog_manager> {
private:
  using StorageType = decltype(init_blog_storage(""));
  unique_ptr<StorageType> storage__;

public:
  blog_manager(const string &path);
};