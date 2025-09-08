#include "host_api_manager.h"
#include "third_party/cpp-dotenv/include/dotenv.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>


int main()
{
    dotenv::env.load_dotenv("../.env");

    auto apiManager =
        api::createHostApiManager()
            .init()
            .loadUsers();

    const std::string uuid = "<some-uuid5>";
    apiManager.getUser("testuser")
        .injectCustomUUID(uuid);

    apiManager.saveUsers();
}
