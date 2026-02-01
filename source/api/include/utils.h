#pragma once


namespace api::utils {

static void EmptyDeleter(void* /*ptr*/) noexcept // NOLINT
{ }

using EmptyDeleterT = decltype(*EmptyDeleter);

} // namespace api::utils
