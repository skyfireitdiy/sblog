//
// Created by skyfire on 19-8-5.
//
#define SF_DEBUG
#include "config_manager.h"
#include <fstream>
#include <sf_utils>

using namespace skyfire;

config_manager::config_manager(const std::string &filename) {
    byte_array data;
    if (!sf_read_file(filename, data)) {
        return;
    }
    config__ = sf_json::from_string(to_string(data));
    inited__ = !config__.is_null();
}

bool config_manager::inited() const { return inited__; }
