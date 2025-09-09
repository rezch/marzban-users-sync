#include "host_api_manager.h"
#include "logging.h"

#include <dotenv.h>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>


int main()
{
    dotenv::env.load_dotenv("../.env");

    LOG_ERROR(dotenv::env["HOST"]);

    auto apiManager =
        api::createHostApiManager()
            .init()
            .loadUsers();

    const std::string uuid = "<some-uuid5>";
    apiManager.getUser("testuser")
        .injectCustomUUID(uuid);

    apiManager.saveUsers();
}
