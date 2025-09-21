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

    // @valid barrier
    HostApiManager& loadUsers();

    // @valid barrier
    HostApiManager& addUser(const user::User& user);

    // @valid barrier
    HostApiManager& saveUsers();

    user::User& getUser(const std::string& name);

    // @valid barrier
    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUser(const std::string& name, Callable visitor);

    // @valid barrier
    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUsers(Callable visitor);

    bool isCompleted();

private:

    bool getToken();

    bool loadUsersImpl();

    bool updateUser(const std::string& name, const user::User& user) const;

    const std::string host_;
    const std::string username_;
    const std::string password_;

    std::optional<std::string> token_;
    std::unordered_map<std::string, user::User> users_;

    bool valid_;
};

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUser(const std::string& name, Callable visitor)
{
    if (!valid_) {
        return *this;
    }
    if (users_.contains(name))
        visitor(users_[name]);
    else
        valid_ = false;
    return *this;
}

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUsers(Callable visitor)
{
    if (!valid_) {
        return *this;
    }
    for (auto& [_, user] : users_) {
        visitor(user);
    }
    return *this;
}

HostApiManager createHostApiManager(std::string host, size_t nodeId);

} // namespace api
