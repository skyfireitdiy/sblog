#pragma once

#include <sflib.h>

using namespace skyfire;

class config_manager
    : public make_instance_t<config_manager, empty_class> {
private:
    skyfire::json config__;
    bool inited__ = false;

    template <int N, typename... Args>
    static std::enable_if_t<sizeof...(Args) - 1 != N, json> value__(
        const json& root, const std::tuple<Args...>& keys,
        const json& default_value = json())
    {
        auto key = std::get<N>(keys);
        static_assert(std::is_convertible_v<decltype(key), int> || std::is_convertible_v<decltype(key), std::string>);
        if constexpr (std::is_convertible_v<decltype(key), int>) {
            auto t_key = static_cast<int>(key);
            if (root.type() != json_type::array) {
                return default_value;
            } else if (root.size() <= t_key) {
                return default_value;
            } else {
                return value__<N + 1>(root.at(t_key), keys, default_value);
            }
        } else if (std::is_convertible_v<decltype(key), std::string>) {
            auto t_key = static_cast<std::string>(key);
            if (root.type() != json_type::object) {
                return default_value;
            } else if (!root.has(t_key)) {
                return default_value;
            } else {
                return value__<N + 1>(root.at(t_key), keys, default_value);
            }
        }
    }

    template <int N, typename... Args>
    static std::enable_if_t<sizeof...(Args) - 1 == N, json> value__(
        const json& root, const std::tuple<Args...>& keys,
        const json& default_value = json())
    {
        auto key = std::get<N>(keys);
        static_assert(std::is_convertible_v<decltype(key), int> || std::is_convertible_v<decltype(key), std::string>);
        if constexpr (std::is_convertible_v<decltype(key), int>) {
            auto t_key = static_cast<int>(key);
            if (root.type() != json_type::array) {
                return default_value;
            } else if (root.size() <= t_key) {
                return default_value;
            } else {
                return root.at(t_key);
            }
        } else if (std::is_convertible_v<decltype(key), std::string>) {
            auto t_key = static_cast<std::string>(key);
            if (root.type() != json_type::object) {
                return default_value;
            } else if (!root.has(t_key)) {
                return default_value;
            } else {
                return root.at(t_key);
            }
        }
    }

public:
    explicit config_manager(const std::string& filename);

    template <typename T>
    [[nodiscard]] skyfire::json value(
        const T& key,
        const skyfire::json& default_value = skyfire::json()) const
    {
        std::tuple<T> keys = { key };
        return value__<0>(config__, keys, default_value);
    }

    template <typename... Args>
    skyfire::json value(
        const std::tuple<Args...>& keys,
        const skyfire::json& default_value = skyfire::json()) const
    {
        return value__<0>(config__, keys, default_value);
    }

    [[nodiscard]] bool inited() const;
};
