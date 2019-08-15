#pragma once

#include "tools/sf_json.hpp"

using namespace skyfire;
using namespace std;

struct blog_config {
    string db_path;
    string static_path;
};

SF_JSONIFY(blog_config, db_path, static_path)
