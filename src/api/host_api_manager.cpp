#include "host_api_manager.h"
#include "logging.h"

#include <cpr/cpr.h>

#define VALID_BARRIER   \
    if (!valid_) {      \
        return *this;   \
    }


namespace api {

HostApiManager::HostApiManager(
        std::string host,
        std::string username,
        std::string password)
    : host_(host),
      username_(username),
      password_(password),
      valid_(true)
{ }

bool HostApiManager::getToken()
{
    const auto url     = cpr::Url{ host_ + "/api/admin/token" };
    const auto headers = cpr::Header{
        { "Content-Type", "application/x-www-form-urlencoded" }
    };
    const auto body = cpr::Payload{
        { "username", username_ },
        { "password", password_ },
    };

    auto r = cpr::Post(url, headers, body);

    if (r.status_code != HTTP_STATUS_OK) {
        LOG_ERROR("Cannot get API token.", "Response code: ", r.status_code);
        return false;
    }

    token_ = nlohmann::json::parse(r.text)["access_token"];
    LOG_DEBUG("Recieved API token");
    return true;
}

HostApiManager& HostApiManager::init()
{
    getToken();
    if (!token_.has_value())
        valid_ = false;
    return *this;
}

bool HostApiManager::loadUsersImpl()
{
    const auto url    = cpr::Url{ host_ + "/api/users" };
    const auto bearer = cpr::Bearer{ token_.value() };

    auto r = cpr::Get(url, bearer);
    if (r.status_code != HTTP_STATUS_OK) {
        LOG_ERROR("Cannot get users info.", "Response code: ", r.status_code);
        return false;
    }

    for (const auto& data : nlohmann::json::parse(r.text)["users"]) {
        const auto username = data["username"];
        users_[username] = user::User(data);
    }

    LOG_DEBUG("Users loaded.");
    return true;
}

HostApiManager& HostApiManager::loadUsers()
{
    if (!token_.has_value()) {
        init();
    }
    VALID_BARRIER;
    if (!loadUsersImpl()) {
        valid_ = false;
    }
    return *this;
}

HostApiManager& HostApiManager::addUser(const user::User& user)
{
    VALID_BARRIER;
    users_[user.getName()] = user;
    return *this;
}

HostApiManager& HostApiManager::saveUsers()
{
    VALID_BARRIER;
    for (const auto& [name, user] : users_) {
        if (!updateUser(name, user)) {
            LOG_ERROR("Failed to save users info to server.");
            valid_ = false;
        }
    }
    return *this;
}

bool HostApiManager::updateUser(const std::string& name, const user::User& user) const
{
    if (user.isSynchronized()) {
        return true;
    }

    const auto url     = cpr::Url{ host_ + "/api/user/" + name };
    const auto bearer = cpr::Bearer{ token_.value() };
    const auto headers = cpr::Header{
        { "Content-Type", "application/json" }
    };
    const auto body = cpr::Body{ user.toShort().dump() };

    auto r = cpr::Put(url, bearer, headers, body);

    if (r.status_code != HTTP_STATUS_OK) {
        LOG_ERROR("Cannot update user info.", "Response code: ", r.status_code, '\n', r.text);
        return false;
    }
    LOG_DEBUG("User info successfully updated.");
    return true;
}

user::User& HostApiManager::getUser(const std::string& name)
{
    return users_[name];
}

HostApiManager createHostApiManager(std::string host, size_t nodeId)
{
    auto strNodeId = std::to_string(nodeId);
    return api::HostApiManager(
        std::move(host),
        dotenv::env["USERNAME_" + strNodeId],
        dotenv::env["PASSWORD_" + strNodeId]);
}

bool HostApiManager::isCompleted()
{
    auto completed = valid_;
    valid_ = true;
    return completed;
}

} // namespace api
