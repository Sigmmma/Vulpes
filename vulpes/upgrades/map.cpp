/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdio>
#include <fstream>
#include <windows.h>

#include <hooker/hooker.hpp>
#include <util/crc32.hpp>

#include <vulpes/memory/types.hpp>

#include "map.hpp"

extern "C" {
    bool is_server = false;
}

// NOP this call if other sigs are found
Signature(false, sig_game_startup_crc_call,
    {0xE8, -1, -1, -1, 0xFF, 0x83, 0xC4, -1, 0xFF, 0x05, -1, -1, -1, -1,
     0x81, 0xC4, 0x00, 0x08, 0x00, 0x00, 0xC3});

// Put our little hook in here that intercepts it when halo asks for a crc
// (Watch out for chimera!)
Signature(true, sig_get_crc_from_table,
    {0x8B, 0x15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
     0x51, 0x89, 0x8C, 0x24, 0xAC, 0x00, 0x00, 0x00});

// Server map CRC
Signature(true, sig_server_map_crc,
    {0xA1, -1, -1, -1, -1, 0xC1, 0xE1, 0x04, 0x8B, 0x44, 0x01, 0x0C, 0xC3, 0xCC});

// I think this can be used for adding maps to the table
Signature(true, sig_add_map_to_list,
    {0x8B, 0x0D, -1, -1, -1, -1, 0x81, 0xEC, 0x00, 0x08, 0x00, 0x00});

// Returns -1 if not found
Signature(true, sig_get_map_from_maps_list,
    {0x56, 0x33, 0xF6, 0xEB, 0x0B, 0x8D, 0xA4, 0x24, 0x00,
     0x00, 0x00, 0x00, 0x8D, 0x64, 0x24, 0x00});

// Replace this function
// This function could be the key to an alternative way
// to fix file handle leaks.
Signature(true, sig_read_map_file_header,
    {0x81, 0xEC, 0x04, 0x01, 0x00, 0x00, 0x53, 0x57});

std::vector<std::string> map_folders;
std::vector<std::string> map_extensions;

std::string default_map_folder("maps\\");

std::string default_map_extension("map");
std::string yelo_map_extension("yelo");

const int HALO_CE_MAP_VERSION = 609;
const uint32_t HALO_MAP_HEADER_HEAD = 0x68656164;
const uint32_t HALO_MAP_HEADER_FOOT = 0x666F6F74;

enum CacheFileType : uint16_t {
    SOLO,
    MP,
    UI
};

struct HaloMapHeader {
    uint32_t head;

    int32_t halo_version;
    int32_t map_size;
    int32_t compressed_map_size;

    int32_t offset_to_tag_index;
    int32_t tag_memory_size;
    PAD(8);
    char name[32];
    char build_string[32];
    CacheFileType cache_type;
    PAD(2);
    uint32_t crc;
    PAD(4);

    PAD(4);
    //YeloMapHeader yelo;// Later we will implement this
                         // so we can support opensauce maps.
    char padding[1940 - 8]; //- (4 + sizeof(YeloMapHeader))];

    uint32_t foot;
}; static_assert( sizeof(HaloMapHeader) == 0x800 );

// Function taken from Chimera 2018 repo - MIT License.
// CRC of map = CRC of BSPs, model data, and tag data
uint32_t calculate_crc32_of_map_file(FILE *f) noexcept {
    uint32_t crc = 0;
    HaloMapHeader header;
    fseek(f, 0, SEEK_SET);
    fread(&header, 1, sizeof(HaloMapHeader), f);

    auto *tag_data = new char[header.tag_memory_size];
    fseek(f, header.offset_to_tag_index, SEEK_SET);
    fread(tag_data, 1, header.tag_memory_size, f);

    auto *scenario_tag = tag_data + (*reinterpret_cast<uint32_t *>(tag_data) - 0x40440000) + (*reinterpret_cast<uint32_t *>(tag_data + 4) & 0xFFFF) * 0x20;
    auto *scenario_tag_data = tag_data + (*reinterpret_cast<uint32_t *>(scenario_tag + 0x14) - 0x40440000);

    // First, the BSP(s)
    auto &structure_bsp_count = *reinterpret_cast<uint32_t *>(scenario_tag_data + 0x5A4);
    auto *structure_bsps = tag_data + (*reinterpret_cast<uint32_t *>(scenario_tag_data + 0x5A4 + 4) - 0x40440000);
    for(size_t b=0;b<structure_bsp_count;b++) {
        char *bsp = structure_bsps + b * 0x20;
        auto &bsp_offset = *reinterpret_cast<uint32_t *>(bsp);
        auto &bsp_size = *reinterpret_cast<uint32_t *>(bsp + 4);

        char *bsp_data = new char[bsp_size];
        fseek(f, bsp_offset, SEEK_SET);
        fread(bsp_data, bsp_size, 1, f);
        crc = crc32(crc, bsp_data, bsp_size);
        delete[] bsp_data;
    }

    // Next, model data
    auto &model_vertices_offset = *reinterpret_cast<uint32_t *>(tag_data + 0x14);
    auto &vertices_size = *reinterpret_cast<uint32_t *>(tag_data + 0x20);

    auto *model_vertices = new char[vertices_size];
    fseek(f, model_vertices_offset, SEEK_SET);
    fread(model_vertices, vertices_size, 1, f);
    crc = crc32(crc, model_vertices, vertices_size);
    delete[] model_vertices;

    // Lastly, tag data
    crc = crc32(crc, tag_data, header.tag_memory_size);
    delete[] tag_data;

    return crc;
}

FILE* open_map_file(const char* name) {
    FILE* map_file;
    char map_path[MAX_PATH];
    int i;
    int j;
    for (i=0; i<map_folders.size(); i++) {
        for (j=0; j<map_extensions.size(); j++) {
            snprintf(&map_path[0], MAX_PATH, "%s%s.%s",
                     map_folders[i].data(), name, map_extensions[j].data());
            map_file = fopen(&map_path[0], "rb");
            if (map_file != NULL) break;
        }
    }
    return map_file;
}

// The upgrades here are that we don't check the size of the map file.
// And we allow for multiple folders and extensions.
extern "C" __attribute__((regparm(2)))
bool read_map_file_header_from_file(const char* map_name,
                          HaloMapHeader* header) {
    bool approved = false;

    FILE* map_file = open_map_file(map_name);
    if (map_file != NULL) {
        // Confirm if we actually got enough data from the file.
        if (fread(header, 1, sizeof(HaloMapHeader), map_file)) {
            // Do all the checks Halo normally does except for the size check.
            if (header->head == HALO_MAP_HEADER_HEAD
            &&  header->foot == HALO_MAP_HEADER_FOOT
            &&  strlen(header->name) < 32
            &&  header->halo_version == HALO_CE_MAP_VERSION) {
                // Purposefully write 0 to this number for
                // if any function calls this to get this data.
                header->map_size = 0;
                approved = true;
            } else {
                approved = false;
            }
        } else {
            approved = false;
        }
        fclose(map_file);
    }
    return approved;
}

struct MapListEntry {
    const char* name;
    int32_t index;
    CacheFileType type;
    PAD(2);
    uint32_t crc;
};

extern "C" __attribute__((regparm(1)))
bool get_map_crc(MapListEntry* entry) {
    if (entry->crc == 0xFFFFFFFF) {
        FILE* map_file = open_map_file(entry->name);
        if (map_file != NULL) {
            entry->crc = ~calculate_crc32_of_map_file(map_file);
            fclose(map_file);
        } else {
            return false;
        }
    }
    printf("Map name: %s, crc: %X\n", entry->name, entry->crc);
    return true;
}

extern "C" {
    uintptr_t* multiplayer_maps_list_ptr;
    uintptr_t jmp_skip_chimera;

    extern read_map_file_header_wrapper();
    extern get_map_crc_wrapper_server();
    extern get_map_crc_wrapper();
}

static bool map_upgrades_initialized = false;
Patch(patch_read_map_file_header_replacement, sig_read_map_file_header, 0, 6, JMP_PATCH, &read_map_file_header_wrapper);
Patch(patch_startup_crc_calc_nop, sig_game_startup_crc_call, 0, 5, NOP_PATCH, 0);
Patch(patch_get_map_crc, sig_get_crc_from_table, 0, 6, CALL_PATCH, &get_map_crc_wrapper);
Patch(patch_get_map_crc_server, sig_server_map_crc, 5, 7, CALL_PATCH, &get_map_crc_wrapper_server);

void init_map_crc_upgrades(bool server) {
    is_server = server;
    if (patch_read_map_file_header_replacement.build()) patch_read_map_file_header_replacement.apply();
    if (patch_startup_crc_calc_nop.build()) patch_startup_crc_calc_nop.apply();
    if (patch_get_map_crc.build()) {
        if (!map_upgrades_initialized) {
            multiplayer_maps_list_ptr = *reinterpret_cast<uintptr_t**>(patch_get_map_crc.address()+2);
            jmp_skip_chimera = patch_get_map_crc.address()+13;
        }
        patch_get_map_crc.apply();
    }
    if (patch_get_map_crc_server.build()) {
        patch_get_map_crc_server.apply();
        if (patch_startup_crc_calc_nop.build()) patch_startup_crc_calc_nop.apply();
    }
    if (!map_upgrades_initialized) {
        map_folders.push_back(default_map_folder);
        map_extensions.push_back(default_map_extension);
        map_upgrades_initialized = true;
    }
}

void revert_map_crc_upgrades() {
    patch_read_map_file_header_replacement.revert();
    patch_startup_crc_calc_nop.revert();
    patch_get_map_crc.revert();
}
