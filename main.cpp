#include "host_api_manager.h"
#include "logging.h"

#include <dotenv.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

int main()
{
    dotenv::env.load_dotenv("../.env");

    std::vector<std::string> names;
    api::createHostApiManager(dotenv::env["HOST"] + "/n1")
        .init()
        .loadUsers()
        .visitUsers(
            [&names](api::user::User& user)
            {
                names.emplace_back(user.getName());
            }
        );

    for (const auto& name : names) {
        LOG_INFO("name:", name);
    }
}
