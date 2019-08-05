#pragma once

#include "tools/sf_json.hpp"

class config_manager {
private:
    skyfire::sf_json config__;
public:
    config_manager(const std::string& filename);

    template <typename ... Args>
    const skyfire::sf_json value(const std::tuple<Args...>& keys, const skyfire::sf_json& default_value = skyfire::sf_json()) const;
    const skyfire::sf_json value(const std::string& key, const skyfire::sf_json& default_value = skyfire::sf_json()) const;
    const skyfire::sf_json value(int key, const skyfire::sf_json& default_value = skyfire::sf_json()) const;

};

