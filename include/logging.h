#pragma once

#include <dotenv.h>

#include <iostream>
#include <source_location>


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

[[maybe_unused]] LOG_LEVEL getLogLevel()
{
    static auto log_level = LOG_LEVEL::UNDEFINED;
    if (log_level == LOG_LEVEL::UNDEFINED) {
        log_level = parseStr(dotenv::env["LOG_LEVEL"]);
    }
    return log_level;
}

static inline std::string getFilePosition(std::source_location location)
{
    return std::string(
               location.file_name())
               + ":"
               + std::to_string(location.line());
}

template <class Tag, class... Args>
void log(Tag &&tag, Args &&...args)
{
    std::cout << "LOG (" << tag << "): ";
    ((std::cout << args << ' '), ...) << std::endl;
}

} // namespace

#define LOG_ERROR(...)                         \
    {                                          \
        if (LOG_LEVEL::ERROR <= getLogLevel()) \
            log("ERROR", __VA_ARGS__);         \
    }

#define LOG_INFO(...)                         \
    {                                         \
        if (LOG_LEVEL::INFO <= getLogLevel()) \
            log("INFO", __VA_ARGS__);         \
    }

#define LOG_DEBUG(...)                         \
    {                                          \
        if (LOG_LEVEL::DEBUG <= getLogLevel()) \
            log("DEBUG", __VA_ARGS__);         \
    }

#define LOG(...)                                                                \
    {                                                                           \
        if (LOG_LEVEL::INFO <= getLogLevel()) {                                 \
            log(getFilePosition(std::source_location::current()), __VA_ARGS__); \
        }                                                                       \
    }
