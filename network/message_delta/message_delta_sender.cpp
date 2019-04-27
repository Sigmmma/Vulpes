#include "message_delta_sender.hpp"
#include "../../hooks/hooker.hpp"

Signature(true, sig_send_message_to_all,
    {0x66, 0x8B, 0x46, 0x0E, 0x8A, 0xD0, 0xD0, 0xEA, 0xF6, 0xC2, 0x01}); // -56

Signature(true, sig_send_message_to_player,
    {0x51, 0x53, 0x57, 0x8B, 0xF8, 0x32, 0xC0, 0x33, 0xC9});

Signature(true, sig_send_message_socket_ready,
    {-1, -1, -1, -1, 0x3B, 0xC3, 0x74, 0x1A, 0x83, 0xC0, 0x08, 0x3B, 0xC3, 0x74, 0x13});

void send_delta_message_to_all(void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority){

    static uintptr_t* socket_ready = (uintptr_t*)*(uintptr_t*)(sig_send_message_socket_ready.get_address());
    static uintptr_t func_send_message_to_all = sig_send_message_to_all.get_address() - 56;

    if(*socket_ready){
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push ecx;\n"
            "push eax;\n"

            "push %8;\n"
            "push %7;\n"
            "push %6;\n"
            "push %5;\n"
            "push %2;\n"
            "push %4;\n"
            "mov ecx, %1;\n"
            "mov eax, %3;\n"
            "call %0;\n"
            "add esp, 0x18;\n"

            "pop eax;\n"
            "pop ecx;\n"
            : "+m" (func_send_message_to_all)   // 0
            : "m" (socket_ready),               // 1
              "m" ((uint32_t)message),          // 2
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

    static uintptr_t* socket_ready = (uintptr_t*)*(uintptr_t*)(sig_send_message_socket_ready.get_address());
    static uintptr_t func_send_message_to_player = sig_send_message_to_player.get_address();

    if(*socket_ready){
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push esi;\n"
            "push eax;\n"

            "push %8;\n"
            "push %7;\n"
            "push %6;\n"
            "push %5;\n"
            "push %3;\n"
            "push %2;\n"
            "push %4;\n"
            "mov esi, %1;\n"
            "mov eax, %9;\n"
            "call %0;\n"
            "add esp, 0x1C;\n"

            "pop eax;\n"
            "pop esi;\n"
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

    for (int i = 0; i< 16; i++){
        if (i != player_id){
            send_delta_message_to_player(i, message, message_size,
                ingame_only, write_to_local_connection,
                flush_queue, unbuffered, buffer_priority);
        };
    };
}
