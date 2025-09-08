#pragma once

#include "third_party/cpp-dotenv/include/dotenv.h"

#include <iostream>


namespace {

enum class LOG_LEVEL {
    UNDEFINED = -1,

    NONE  = 0,
    ERROR = 1,
    INFO  = 2,
    DEBUG = 3,
};

inline LOG_LEVEL parseStr(const std::string &log_level)
{
    if (log_level == "INFO") {
        return LOG_LEVEL::INFO;
    }
    if (log_level == "DEBUG") {
        return LOG_LEVEL::DEBUG;
    }
    return LOG_LEVEL::NONE;
}

[[ maybe_unused ]] LOG_LEVEL getLogLevel()
{
    static auto log_level = LOG_LEVEL::UNDEFINED;
    if (log_level == LOG_LEVEL::UNDEFINED) {
        log_level = parseStr(dotenv::env["LOG_LEVEL"]);
    }
    return log_level;
}

template <class... Args>
void log(Args&&... args)
{
    std::cout << "LOG: ";
    ((std::cout << args << ' '), ...) << std::endl;
}

} // namespace

#define LOG_ERROR(...) \
{ if (LOG_LEVEL::ERROR <= getLogLevel()) log(__VA_ARGS__); }

#define LOG_INFO(...) \
{ if (LOG_LEVEL::INFO <= getLogLevel()) log(__VA_ARGS__); }

#define LOG_DEBUG(...) \
{ if (LOG_LEVEL::DEBUG <= getLogLevel()) log(__VA_ARGS__); }
