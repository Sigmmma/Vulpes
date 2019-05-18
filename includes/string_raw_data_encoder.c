#include <string.h>
#include <stdint.h>

const uint16_t MASKS16[] = {
    1,
    2,
    4,
    8,
    0x10,
    0x20,
    0x40,
    0x80,
    0x100,
    0x200,
    0x400,
    0x800,
    0x1000,
    0x2000,
    0x4000,
    0x8000
};
// Encodes an array of 16 bit ints with the size len into a wchar_t array.
// Adding encoding info at the end.
// Warning: Output len can be up to len*16/15 as long as input (longer!).
size_t wstr_raw_data_encode(wchar_t* dest, const uint16_t* src, size_t len){
    // Copy the data if the destination isn't the source
    if ((void*)dest != (void*)src){
        for (int i=0;i<len;i++){
            dest[i] = src[i];
        };
    };
    // Make sure there isn't any 0s. Add encoding info at the back.
    size_t enc_len = 0;
    int16_t cur_meta;
    for (int i=0;i<len;i++){
        int bit = i%15;
        // if bit == 0 that means we should copy our encoding info to the end
        // of the string, then reset and start a new encoding mask.
        if (bit==0){
            dest[len+enc_len] = cur_meta;
            enc_len++;
            // We always set the last bit to true.
            // This is to avoid more logic
            // and having any 0s cutting off our encoding data.
            cur_meta = MASKS16[15];
        };
        // If the data == 0 then add 1 and indicate that we did this
        // in the encoding mask.
        if (dest[i] == 0){
            cur_meta |= MASKS16[bit];
            dest[i]++;
        };
    };
    // NULL terminate.
    enc_len++;
    dest[len+enc_len-1] = 0;
    // Cut off any useless fluff.
    // If the mask equals mask 15 then there is no data encoded at this
    // index. And we should just be able to derive this.
    while (dest[len+enc_len-1] == MASKS16[15] && enc_len > 0){
        dest[len+enc_len-1] = 0;
        enc_len--;
    };
    return len+enc_len;
}

// Decodes a wchar_t array into a uint16_t array.
void wstr_raw_data_decode(uint16_t* dest, const wchar_t* src, size_t len){
    // Copy the data if the destination isn't the source
    if ((void*)dest != (void*)src){
        for (int i=0;i<len;i++){
            dest[i] = src[i];
        };
    };
    for (int i=0;i<len;i++){
        int m = i%15; // mask index
        int e = 1+i/15; // encoding index
        // If we find a NULL terminator there is no more encoded data to
        // go through.
        if (src[len+e] == 0){
            break;
        };
        // Decrement the integer at the id if the encoding info indicates
        // one was added.
        if (src[len+e] & MASKS16[m]){
            dest[i]--;
        };
    };
}
