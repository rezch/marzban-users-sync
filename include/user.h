#pragma once

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

    User& injectCustomUUID(const std::string& uuid);

    nlohmann::json toShort() const;

    nlohmann::json& operator[](std::string&& key);
    nlohmann::json& operator[](const std::string& key);

private:
    nlohmann::json data_;
    bool synchronized_;
};

} // namespace api::user
