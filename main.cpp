#include "host_api_manager.h"
#include "logging.h"

#include <dotenv.h>

int main()
{
    dotenv::env.load_dotenv("../.env");

    std::unordered_map<std::string, api::user::User> users;
    api::createHostApiManager(dotenv::env["HOST"] + "/n1", 1)
        .init()
        .loadUsers()
        .visitUsers(
            [&](api::user::User user)
            {
                users[user.getName()] = user;
            }
        );
}
