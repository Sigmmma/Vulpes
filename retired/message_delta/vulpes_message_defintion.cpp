/*
__attribute__((cdecl))
void handle_vulpes_message(MessageDeltaHeader* header){
    VulpesMessage decoded_message;
    printf("Test packet was received!\n");
    if (mdp_decode_stateless_iterated((void*)&decoded_message, header)){
        printf("Got a length of: %d\nPayload contained:\n", decoded_message.payload_size);
        for (int i = 0; i<decoded_message.payload_size; i++){
            printf("%X ", decoded_message.payload8[i]);
        };
        printf("\n");
    };
    mdp_discard_iteration_body(header);
}

__attribute__((cdecl))
void encode_vulpes_message(const MessageDeltaFieldPropertyDefinition* const definition,
                           int32_t        bytes_left,
                           VulpesMessage* const input,
                           void* const    iteration_header){ // handles output buffer

    printf("Vulpes message encoder was called!\n");
    bitstream_write(iteration_header, &input->payload_size, 16);

    bytes_left = input->payload_size;
    printf("Payload size we got was: %d\n", input->payload_size);
    printf("Payload we got was as:\n");
    for (int i = 0; i<input->payload_size; i++){
        printf("%X ", input->payload8[i]);
    };
    for (int i=0; i<input->payload_size; i+=4) { // esi, edx, esp+4
        int32_t num_write_bytes = (input->payload_size - i) % 4;
        if (num_write_bytes == 0){
            num_write_bytes = 4;
        };
        printf("Calling bitstream write.\n");
        bitstream_write(iteration_header, &input->payload32[i], num_write_bytes * 8);
    };
    printf("Vulpes message encoder got done executing!\n");
}

__attribute__((cdecl))
void decode_vulpes_message(const MessageDeltaFieldPropertyDefinition* const definition,
                           int32_t        _dummy,
                           VulpesMessage* const output,
                           void* const    iteration_header){ // handles input buffer

    printf("Vulpes message decoder was called!\n");
    bitstream_read(iteration_header, &output->payload_size, 16);
    // returns integer containing decoded bits
    for (int i=0; i<output->payload_size; i+=4) { // esi, edx, esp+4
        int32_t num_read_bytes = (output->payload_size - i) % 4;
        if (num_read_bytes == 0){
            num_read_bytes = 4;
        };
        bitstream_read(iteration_header, &output->payload32[i], num_read_bytes * 8);
    };
}
static int32_t nothing;

MessageDeltaFieldPropertyDefinition vulpes_message_field_properties;

struct vulpes_message_definition {
    MessageDeltaDefinition header;
    MessageDeltaFieldReferenceDefinition body;
} vulpes_msg_def;

int32_t header_field_set[10];

void init_vulpes_message_delta(){
    header_field_set[1] = -1;
    vulpes_msg_def.header.type = 0x39;
    vulpes_msg_def.header.initialized = true;
    vulpes_msg_def.header.header_field_set = header_field_set;
    vulpes_msg_def.header.body_field_set.field_count = 1;

    char str_vulpes_message_data[] = "vulpes_message_data";


    vulpes_msg_def.body.properties = &vulpes_message_field_properties;
    vulpes_msg_def.body.properties->type    = 7; // 7 stands for arbitrary data TODO: Make enum
    strncpy(reinterpret_cast<char*>(&vulpes_msg_def.body.properties->name), str_vulpes_message_data, 0x4C);
    vulpes_msg_def.body.properties->encoder = reinterpret_cast<void*>(&encode_vulpes_message);
    vulpes_msg_def.body.properties->decoder = reinterpret_cast<void*>(&decode_vulpes_message);
    vulpes_msg_def.body.properties->parameters = &nothing;
    vulpes_msg_def.body.properties->max_size = 516;
    vulpes_msg_def.body.properties->initialized = false;
}

uintptr_t get_vulpes_message_definition(){
    init_vulpes_message_delta();
    return reinterpret_cast<uintptr_t>(&vulpes_msg_def);
}
*/