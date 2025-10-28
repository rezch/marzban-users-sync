#pragma once

#include <dotenv.h>

#include <iostream>


namespace {

enum class LOG_LEVEL {
    UNDEFINED = -1,

    NONE  = 0,
    ERROR = 1,
    INFO  = 2,
    DEBUG = 3,
};

inline LOG_LEVEL parseStr(const std::string& log_level)
{
    if (log_level == "ERROR") {
        return LOG_LEVEL::ERROR;
    }
    if (log_level == "INFO") {
        return LOG_LEVEL::INFO;
    }
    if (log_level == "DEBUG") {
        return LOG_LEVEL::DEBUG;
    }
    return LOG_LEVEL::NONE;
}

LOG_LEVEL getLogLevel()
{
    static auto log_level = LOG_LEVEL::UNDEFINED;
    if (log_level == LOG_LEVEL::UNDEFINED) {
        log_level = parseStr(dotenv::env["LOG_LEVEL"]);
    }
    return log_level;
}

template <class... Args>
void log(Args&&...args)
{
    ((std::cout << args << ' '), ...) << std::endl;
}

template <class Tag, class... Args>
void logTag(Tag&& tag, Args&&... args)
{
    std::cout << "LOG (" << tag << "): ";
    log(args...);
}

} // namespace

#define LOG_ERROR(...)                          \
    {                                           \
        if (LOG_LEVEL::ERROR <= getLogLevel())  \
            logTag("ERROR", __VA_ARGS__);       \
    }

#define LOG_INFO(...)                           \
    {                                           \
        if (LOG_LEVEL::INFO <= getLogLevel())   \
            logTag("INFO", __VA_ARGS__);        \
    }

#define LOG_DEBUG(...)                          \
    {                                           \
        if (LOG_LEVEL::DEBUG <= getLogLevel())  \
            logTag("DEBUG", __VA_ARGS__);       \
    }

#define LOG(...) log("LOG:", __VA_ARGS__);
