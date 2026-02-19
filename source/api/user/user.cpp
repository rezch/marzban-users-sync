#include "user.h"
#include "logging.h"


#define LOG_USER(...) \
    LOG_DEBUG("User:", getName(), __VA_ARGS__);


namespace api::user {

namespace {
std::unordered_set<std::string> parseAliases(std::string_view note)
{
    return {};
}
} // namespace

User::User(const nlohmann::json& data)
    : data_(data),
      synchronized_(true)
{
    parseAliases();
}

User::User(const nlohmann::json& data,
         bool synchronized)
    : data_(data),
      synchronized_(synchronized)
{
    parseAliases();
}

void User::parseAliases()
{
    if (!data_.contains("note") || data_["note"].is_null())
        return;

    try {
        auto note = nlohmann::json::parse(std::string(data_["note"]));
    }
    catch (nlohmann::json::parse_error& e) {}
}

std::string User::getName() const
{
    return data_["username"];
}

bool User::isSynchronized() const
{
    return synchronized_;
}

User& User::injectProxyUUID(const std::string& proxy, const std::string& uuid)
{
    LOG_USER("inject uuid:", uuid);
    synchronized_ = false;
    data_["proxies"][proxy]["id"] = uuid;
    return *this;
}

nlohmann::json User::forPost() const
{
    nlohmann::json shortData = data_;
    shortData.erase("username");
    return shortData;
}

} // namespace api::user
