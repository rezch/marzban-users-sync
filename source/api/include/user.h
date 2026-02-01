#pragma once

#include <unordered_set>
#define JSON_HAS_RANGES 1
#include <nlohmann/json.hpp>

#include <string>


namespace api::user {

class User {
public:
    User() = default;

    User(const nlohmann::json& data);

    User(const nlohmann::json& data,
         bool synchronized);

    std::string getName() const;

    bool isSynchronized() const;

    User& injectCustomUUID(const std::string& proxy, const std::string& uuid);

    nlohmann::json toShort() const;

    template <class Key>
    nlohmann::json& operator[](Key&& key);

private:
    void parseAliases();

    nlohmann::json data_;
    std::unordered_set<std::string> aliases_;
    bool synchronized_;
};

template <class Key>
nlohmann::json& User::operator[](Key&& key)
{
    synchronized_ = false;
    return data_[key];
}

} // namespace api::user
