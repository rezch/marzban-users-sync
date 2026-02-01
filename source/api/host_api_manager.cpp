#include "host_api_manager.h"
#include "logging.h"
#include "utils.h"

#include <cpr/cpr.h>


namespace {
constexpr int HTTP_STATUS_OK = 200;
}; // namespace

namespace api {

HostApiManager::HostApiManager(
    std::string host,
    std::string username,
    std::string password)
    : host_(host),
      username_(username),
      password_(password),
      valid_(true),
      self(this, utils::EmptyDeleter)
{ }

HostApiManager& HostApiManager::init()
{
    return defer(
        [](HostApiManager* self)
        {
            self->getToken();
            if (!self->token_.has_value())
                self->valid_ = false;
        });
}

HostApiManager& HostApiManager::run()
{
    if (deferredOp_)
        deferredOp_();
    deferredOp_ = nullptr;
    return *this;
}

HostApiManager& HostApiManager::clear()
{
    deferredOp_ = nullptr;
    valid_      = true;
    return *this;
}

bool HostApiManager::isCompleted()
{
    return !deferredOp_ && valid_;
}

bool HostApiManager::getToken()
{
    LOG_ERROR("---", host_);

    const auto url     = cpr::Url { host_ + "/api/admin/token" };
    const auto headers = cpr::Header {
        { "Content-Type", "application/x-www-form-urlencoded" }
    };
    const auto body = cpr::Payload {
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

bool HostApiManager::loadUsersImpl()
{
    init();
    const auto url    = cpr::Url { host_ + "/api/users" };
    const auto bearer = cpr::Bearer { token_.value() };

    auto r = cpr::Get(url, bearer);
    if (r.status_code != HTTP_STATUS_OK) {
        LOG_ERROR("Cannot get users info.", "Response code: ", r.status_code);
        return false;
    }

    for (const auto& data : nlohmann::json::parse(r.text)["users"]) {
        const auto username = data["username"];
        users_[username]    = user::User(data);
    }

    LOG_DEBUG("Users loaded.");
    return true;
}

HostApiManager& HostApiManager::loadUsers()
{
    return defer(
        [](HostApiManager* self)
        {
            if (!self->loadUsersImpl())
                self->valid_ = false;
        });
}

HostApiManager& HostApiManager::addUser(const user::User& user)
{
    return defer(
        [](HostApiManager* self, const user::User& user)
        {
            self->users_[user.getName()] = user;
        },
        user);
}

HostApiManager& HostApiManager::saveUsers()
{
    return visitUsers(
        [this](user::User& user)
        {
            auto name = user.getName();
            if (!updateUser(name, user)) {
                LOG_ERROR("Failed to save info to server for user: ", name);
                valid_ = false;
            }
        });
}

bool HostApiManager::updateUser(const std::string& name, const user::User& user)
{
    if (user.isSynchronized()) {
        return true;
    }

    init();
    const auto url     = cpr::Url { host_ + "/api/user/" + name };
    const auto bearer  = cpr::Bearer { token_.value() };
    const auto headers = cpr::Header {
        { "Content-Type", "application/json" }
    };
    const auto body = cpr::Body { user.toShort().dump() };

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

std::unique_ptr<HostApiManager> createHostApiManager()
{
    return std::make_unique<api::HostApiManager>(
        dotenv::env["HOST"],
        dotenv::env["USERNAME"],
        dotenv::env["PASSWORD"]);
}

} // namespace api
