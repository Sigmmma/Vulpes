#include "message_delta.hpp"
#include "../../hooker/hooker.hpp"

Signature(true, sig_mdp_decode_stateless_iterated,
    {0x57, 0x8B, 0x38, 0x51, 0x83, 0xC0, 0x04, 0x6A, 0x00, 0x50, 0xE8});

Signature(true, sig_mdp_encode_stateless_iterated,
    {0x81, 0xEC, 0xA4, 0x00, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC, 0x24,
     0xBC, 0x00, 0x00, 0x00});

Signature(true, sig_mdp_discard_iteration_body,
    {0x81, 0xEC, 0x00, 0x08, 0x00, 0x00, 0x56, 0x57, 0x8B, 0x38, 0x8B, 0x37});

static uintptr_t func_mdp_encode;
static uintptr_t func_mdp_decode;
static uintptr_t func_mdp_discard;

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, int32_t arg1, MessageDeltaType type,
    uint32_t arg3, void* unencoded_message, uint32_t arg5, uint32_t arg6, uint32_t arg7){

    uint32_t output_size;
    int32_t type_int = type;
    void** ptr_to_ptr = &unencoded_message;
    asm (
        "pushad;"
        "push %8;"
        "push %7;"
        "push %6;"
        "push %5;"
        "push %4;"
        "push %3;"
        "push %2;"
        "mov edx, 0x7FF8;"
        "mov eax, %1;"
        "call %0;"
        "add esp, 0x1C;"
        "mov %9, eax;"
        "popad;"
        : "+m" (func_mdp_encode)
        : "m" (output_buffer),
          "m" (arg1),
          "m" (type_int),
          "m" (arg3),
          "m" (ptr_to_ptr),
          "m" (arg5),
          "m" (arg6),
          "m" (arg7),
          "m" (output_size)
    );
    return output_size;
}

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, MessageDeltaType type, void* unencoded_message){

    return mdp_encode_stateless_iterated(output_buffer, 0, type, 0, unencoded_message, 0, 1, 0);
}
// Test this
bool mdp_decode_stateless_iterated(void* destination, MessageDeltaHeader* message_header){
    bool success;
    asm (
        "mov ecx, %1;"
        "mov eax, %2;"
        "call %0;"
        "mov %3, al;"
        : "+m" (func_mdp_decode)
        : "m" (destination),
          "m" ((void*)message_header),
          "m" (success)
    );
    return success;
}
// Test this
void mdp_discard_iteration_body(MessageDeltaHeader* message_header){
    asm (
        "pushad;"
        "mov eax, %1;"
        "call %0;"
        "popad;"
        :
        : "m" (func_mdp_discard), "m" ((void*)message_header)
    );
}

void init_message_delta_processor(){
    func_mdp_encode = sig_mdp_encode_stateless_iterated.get_address();
    func_mdp_decode = sig_mdp_decode_stateless_iterated.get_address();
    func_mdp_discard = sig_mdp_discard_iteration_body.get_address();
}

Signature(true, sig_send_message_to_all,
    {0x66, 0x8B, 0x46, 0x0E, 0x8A, 0xD0, 0xD0, 0xEA, 0xF6, 0xC2, 0x01}); // -56

Signature(true, sig_send_message_to_player,
    {0x51, 0x53, 0x57, 0x8B, 0xF8, 0x32, 0xC0, 0x33, 0xC9});

Signature(true, sig_send_message_socket_ready,
    {-1, -1, -1, -1, 0x3B, 0xC3, 0x74, 0x1A, 0x83, 0xC0, 0x08, 0x3B, 0xC3, 0x74, 0x13});

static uintptr_t* socket_ready;
static uintptr_t func_send_message_to_all;
static uintptr_t func_send_message_to_player;

void send_delta_message_to_all(void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority){

    if(*socket_ready){
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push ecx;"
            "push eax;"

            "push %8;"
            "push %7;"
            "push %6;"
            "push %5;"
            "push %2;"
            "push %4;"
            "mov ecx, %1;"
            "mov ecx, [ecx];"
            "mov eax, %3;"
            "call %0;"
            "add esp, 0x18;"

            "pop eax;"
            "pop ecx;"
            :
            : "m" (func_send_message_to_all),   // 0
              "m" (socket_ready),               // 1
              "m" (message),                    // 2
              "m" (message_size),               // 3
              "m" (bool_ingame_only),           // 4
              "m" (bool_write_to_local_connection),//5
              "m" (bool_flush_queue),           // 6
              "m" (bool_unbuffered),            // 7
              "m" (buffer_priority)             // 8
        );
    };
}

void send_delta_message_to_player(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority){

    if(*socket_ready){
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push esi;"
            "push eax;"

            "push %8;"
            "push %7;"
            "push %6;"
            "push %5;"
            "push %3;"
            "push %2;"
            "push %4;"
            "mov esi, %1;"
            "mov esi, [esi];"
            "mov eax, %9;"
            "call %0;"
            "add esp, 0x1C;"

            "pop eax;"
            "pop esi;"
            : "+m" (func_send_message_to_player)// 0
            : "m" (socket_ready),               // 1
              "m" ((uint32_t)message),          // 2
              "m" (message_size),               // 3
              "m" (bool_ingame_only),           // 4
              "m" (bool_write_to_local_connection),//5
              "m" (bool_flush_queue),           // 6
              "m" (bool_unbuffered),            // 7
              "m" (buffer_priority),            // 8
              "m" (player_id)                   // 9
        );
    };
}

void send_delta_message_to_all_except(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority){

    for (int i = 0; i<16; i++){
        if (i != player_id){
            send_delta_message_to_player(i, message, message_size,
                ingame_only, write_to_local_connection,
                flush_queue, unbuffered, buffer_priority);
        };
    };
}

void init_message_delta_sender(){
    socket_ready = (uintptr_t*)*(uintptr_t*)(sig_send_message_socket_ready.get_address());
    func_send_message_to_all = sig_send_message_to_all.get_address() - 56;
    func_send_message_to_player = sig_send_message_to_player.get_address();
}
