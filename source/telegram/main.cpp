#include "logging.h"
#include "utils.h"

#include <filesystem>
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <dotenv.h>


int main()
{
    dotenv::env.load_dotenv(CURRENT_PATH() / "../../.env");

    std::string token = dotenv::env["TELEGRAM_API_TOKEN"];
    if (token.empty()) {
        LOG_ERROR("NO TOKEN FOUND\n");
        return 1;
    }

    TgBot::Bot bot(token);

    bot.getEvents().onCommand("start",
        [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Hi!");
        });

    bot.getEvents().onCommand("test",
        [&bot](TgBot::Message::Ptr message)
        {
            printf("User wrote %s\n", message->text.c_str());
            bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        });

    try {
        LOG_INFO("Bot username:", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);

        while (true) {
            LOG_INFO("Long poll started...");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        LOG_ERROR("error:", e.what());
    }
    return 0;
}
