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

    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUser(const std::string& name, Callable visitor);

    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUsers(Callable visitor);

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

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUser(const std::string& name, Callable visitor)
{
    if (users_.contains(name))
        visitor(users_[name]);
    return *this;
}

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUsers(Callable visitor)
{
    for (auto& [_, user] : users_) {
        visitor(user);
    }
    return *this;
}

HostApiManager createHostApiManager(std::string host);

} // namespace api
