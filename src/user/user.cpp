#include "user.h"
#include "logging.h"


namespace api::user {

User::User(const nlohmann::json& data)
    : data_(data),
      synchronized_(true)
{ }

User::User(const nlohmann::json& data,
         bool synchronized)
    : data_(data),
      synchronized_(synchronized)
{ }

std::string User::getName() const
{
    return data_["username"];
}

bool User::isSynchronized() const
{
    return synchronized_;
}

User& User::injectCustomUUID(const std::string& uuid)
{
    LOG_DEBUG("User:", getName(), "inject uuid:", uuid);
    synchronized_ = false;
    data_["proxies"]["vless"]["id"] = uuid;
    return *this;
}

nlohmann::json User::toShort() const
{
    return {
        { "proxies", data_["proxies"] },
        { "inbounds", data_["inbounds"] },
        { "expire", data_["expire"] },
        { "data_limit", data_["data_limit"] },
        { "data_limit_reset_strategy", data_["data_limit_reset_strategy"] },
        { "status", data_["status"] },
        { "note", data_["note"] },
        { "on_hold_timeout", data_["on_hold_timeout"] },
        { "on_hold_expire_duration", data_["on_hold_expire_duration"] },
    };
}

} // namespace api::user
