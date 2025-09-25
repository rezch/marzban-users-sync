#pragma once

#include "user.h"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <unordered_map>


namespace api {

struct SystemStats {
    std::string version;
    uint32_t mem_total;
    uint32_t mem_used;
    uint32_t cpu_cores;
    uint32_t cpu_usage;
    uint32_t total_user;
    uint32_t users_active;
    uint32_t incoming_bandwidth;
    uint32_t outgoing_bandwidth;
    uint32_t incoming_bandwidth_speed;
    uint32_t outgoing_bandwidth_speed;
};

class HostApiManager {
    using jsonNode = std::unordered_map<std::string, nlohmann::json>;

public:
    HostApiManager(
            std::string host,
            std::string username,
            std::string password);

    HostApiManager& init();

    // *** Users Api ***

    /* @valid barrier */
    HostApiManager& loadUsers();

    /* @valid barrier */
    HostApiManager& addUser(const user::User& user);

    /* @valid barrier */
    HostApiManager& saveUsers();

    user::User& getUser(const std::string& name);

    /* @valid barrier */
    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUser(const std::string& name, Callable visitor);

    /* @valid barrier */
    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUsers(Callable visitor);

    // *** System stats Api ***
    SystemStats getSystemStats() const;

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
