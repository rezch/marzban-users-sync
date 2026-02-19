#pragma once

#include <filesystem>


namespace utils {

static void EmptyDeleter(void* /*ptr*/) noexcept // NOLINT
{ }

using EmptyDeleterT = decltype(*EmptyDeleter);

} // namespace utils

#define CURRENT_PATH() \
    std::filesystem::path(__FILE__).parent_path()
