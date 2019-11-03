#pragma once

#include <cstdio>
#include <memory>
#include <string>
#include <windows.h>

static inline size_t file_get_size(FILE* f){
    // Get current position.
    size_t pos = ftell(f);
    // Go to the end.
    fseek(f, 0, SEEK_END);
    // Use the SEEK_END position as the size.
    size_t size = ftell(f);
    // Set position back to what it was before this call.
    fseek(f, pos, SEEK_SET);
    // Return the size.
    return size;
}

template<typename T>
static inline void file_read_into_buffer(T buff, FILE* f){
    // Get current position.
    size_t pos = ftell(f);
    // Get size
    size_t size = file_get_size(f);
    // Set file position to start.
    fseek(f, 0, SEEK_SET);
    // Read file into buffer.
    fread(buff, size, 1, f);
    // Set file position to what it was before.
    fseek(f, pos, SEEK_SET);
}

static inline bool is_dir(std::string path){
    DWORD ftyp = GetFileAttributes(path.data());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

static inline bool is_file(std::string path){
    DWORD ftyp = GetFileAttributes(path.data());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return false;
    return true;
}

static inline void make_dir(std::string path){
    CreateDirectory(path.data() , 0);
}
