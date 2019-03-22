struct packet_header {
    uint8_t payload_size;
    uint8_t timestamp;
}; static_assert(sizeof(packet_header) == 2,
    "Packet header size isn't 2.");

const uint32_t MAX_ENCODING_BYTES = 30;
const uint32_t MAX_PAYLOAD_BYTES = 254 - MAX_ENCODING_BYTES - sizeof(packet_header);
const uint8_t TIMESTAMP_DIFFERENCE_THRESHOLD = 7*30;
const uint8_t MAX_TIME_SINCE_LAST_ACCEPTED_PACKET = 1*30;

static uint8_t packet_buffer[256];

static_assert(MAX_PAYLOAD_BYTES + MAX_ENCODING_BYTES >= sizeof(packet_buffer) - sizeof(packet_header) - 2,
    "MAX_PAYLOAD_BYTES + MAX_ENCODING_BYTES exceeds the maximum space they are allowed to take up in the buffer.");
static_assert(MAX_PAYLOAD_BYTES / MAX_ENCODING_BYTES <= MAX_ENCODING_BYTES,
    "MAX_PAYLOAD_BYTES cannot be encoded using MAX_ENCODING_BYTES.");
