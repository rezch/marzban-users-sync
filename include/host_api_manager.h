#pragma once

#include "user.h"
#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <unordered_map>


namespace api {

namespace {
constexpr int HTTP_STATUS_OK = 200;
} // namespace

class HostApiManager {
    using jsonNode = std::unordered_map<std::string, nlohmann::json>;

public:
    HostApiManager(
            std::string host,
            std::string username,
            std::string password);

    HostApiManager& init();

    HostApiManager& loadUsers();

    user::User& getUser(const std::string& name);

    bool saveUsers() const;

private:
    void getToken();

    bool updateUser(const std::string& name, const user::User& user) const;

    const std::string host_;
    const std::string username_;
    const std::string password_;

    std::optional<std::string> token_;
    std::unordered_map<std::string, user::User> users_;
};

HostApiManager createHostApiManager();

} // namespace api
