#include "host_api_manager.h"
#include "logging.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>


namespace api {

HostApiManager::HostApiManager(
        std::string host,
        std::string username,
        std::string password)
    : host_(host),
      username_(username),
      password_(password)
{ }

void HostApiManager::getToken()
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
    }
    else {
        LOG_DEBUG("Recieved API token");
        token_ = nlohmann::json::parse(r.text)["access_token"];
    }
}

HostApiManager& HostApiManager::init()
{
    getToken();
    return *this;
}

HostApiManager& HostApiManager::loadUsers()
{
    if (!token_.has_value()) {
        return *this;
    }

    const auto url    = cpr::Url{ host_ + "/api/users" };
    const auto bearer = cpr::Bearer{ token_.value() };

    auto r = cpr::Get(url, bearer);
    if (r.status_code != HTTP_STATUS_OK) {
        LOG_ERROR("Cannot get users info.", "Response code: ", r.status_code);
        return *this;
    }

    auto usersData = nlohmann::json::parse(r.text)["users"];
    for (const auto& data : usersData) {
        const auto username = data["username"];
        users_[username] = user::User(data);
    }

    LOG_DEBUG("Users loaded.");
    return *this;
}

user::User& HostApiManager::getUser(const std::string& name)
{
    return users_[name];
}

bool HostApiManager::saveUsers() const
{
    for (const auto& [name, user] : users_) {
        if (!updateUser(name, user)) {
            LOG_ERROR("");
            return false;
        }
    }
    return true;
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

HostApiManager createHostApiManager(std::string host)
{
    return api::HostApiManager(
        std::move(host),
        dotenv::env["USERNAME"],
        dotenv::env["PASSWORD"]);
}

} // namespace api
