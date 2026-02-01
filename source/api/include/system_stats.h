#pragma once

#include <cstdint>
#include <string>


namespace api {

struct SystemStats {
    std::string version;
    uint32_t mem_total;
    uint32_t mem_used;
    uint32_t cpu_cores;
    uint32_t cpu_usage;
    uint32_t total_user;
    uint32_t users_active;
    uint32_t incoming_bandwidth;
    uint32_t outgoing_bandwidth;
    uint32_t incoming_bandwidth_speed;
    uint32_t outgoing_bandwidth_speed;
};

} // namespace api
