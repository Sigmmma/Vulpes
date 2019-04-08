Signature(true, sig_connection_type,
    {0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x75, -1, 0x8B});
enum class ConnectionType : uint32_t {
    NONE = 0,
    CLIENT = 1,
    HOST = 2
};

ConnectionType* connection_type = reinterpret_cast<ConnectionType*>(sig_connection_type.get_address() + 3);
