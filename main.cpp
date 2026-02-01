#include "host_api_manager.h"
#include "logging.h"

#include <dotenv.h>


int main()
{
    dotenv::env.load_dotenv("../.env");

    auto hostApi = api::createHostApiManager();

    hostApi->init()
        .loadUsers()
        .visitUser("test",
            [&](api::user::User& user)
            {
                user.injectCustomUUID(
                    "vless",
                    "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
            }
        )
        .saveUsers();

    LOG("Start");
    if (hostApi->run().isCompleted())
        LOG("Done");
    else
        LOG("Got an error during execution");
}
