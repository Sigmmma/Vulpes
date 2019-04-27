#pragma once
#include <cstdint>

enum class ConnectionType : uint32_t {
    NONE = 0,
    CLIENT = 1,
    HOST = 2
};

ConnectionType get_connection_type();
