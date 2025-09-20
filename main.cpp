#include "host_api_manager.h"
#include "logging.h"

#include <dotenv.h>


int main()
{
    dotenv::env.load_dotenv("../.env");

    std::unordered_map<std::string, api::user::User> users;
    auto hostApi =
        api::createHostApiManager(dotenv::env["HOST"] + "/n2", 2)
        .init()
        .loadUsers()
        .visitUser("test",
            [&](api::user::User& user)
            {
                user.injectCustomUUID("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
            }
        )
        .saveUsers();

    if (hostApi.isCompleted()) {
        LOG("Done");
    }
}
