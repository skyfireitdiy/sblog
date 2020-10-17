//
// Created by skyfire on 19-8-5.
//
#include "config_manager.h"
#include <fstream>
#include <sf_utils>

using namespace skyfire;

config_manager::config_manager(const std::string &filename) {
    byte_array data;
    if (!read_file(filename, data)) {
        return;
    }
    config__ = json::from_string(to_string(data));
    inited__ = !config__.is_null();
}

bool config_manager::inited() const { return inited__; }
