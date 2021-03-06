/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <cstdio>
#include <fstream>
#include <windows.h>

#include <hooker/hooker.hpp>
#include <util/crc32.hpp>

#include <vulpes/memory/types.hpp>
#include <vulpes/memory/signatures.hpp>

#include "map.hpp"

extern "C" {
    bool is_server = false;
}

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
// The Ballmer peak is followed by a slope twice
// as steep as the one leading up to it.
static Patch(patch_read_map_file_header_replacement, NULL, 6,
    JMP_PATCH, &read_map_file_header_wrapper);
// NOP this call if other sigs are found
static Patch(patch_startup_crc_calc_nop, NULL, 5,
    NOP_PATCH, 0);
// Put our little hook in here that intercepts it when halo asks for a crc
// (Watch out for chimera!)
static Patch(patch_get_map_crc, NULL, 6,
    CALL_PATCH, &get_map_crc_wrapper);
static Patch(patch_get_map_crc_server, NULL, 7,
    CALL_PATCH, &get_map_crc_wrapper_server);

void init_map_crc_upgrades(bool server) {
    auto sig_addr1 = sig_map_crc_game_startup_call();
    auto sig_addr2 = sig_map_crc_read_map_file_header();
    is_server = server;
    if (patch_read_map_file_header_replacement.build(sig_addr2))
        patch_read_map_file_header_replacement.apply();
    if (patch_startup_crc_calc_nop.build(sig_addr1))
        patch_startup_crc_calc_nop.apply();
    if (patch_get_map_crc.build(sig_map_crc_get_crc_from_table_hook())) {
        if (!map_upgrades_initialized) {
            // Get pointer to the map list from the original code before we
            // overwrite it.
            multiplayer_maps_list_ptr = *reinterpret_cast<uintptr_t**>(
                patch_get_map_crc.address() + 2);
            // This marks the location we need to jump to to avoid chimera
            // doing the same if we were succesful.
            // Its patch lives before this!
            jmp_skip_chimera = patch_get_map_crc.address() + 13;
        }
        patch_get_map_crc.apply();
    }
    if (patch_get_map_crc_server.build(
            sig_map_crc_server_get_crc_from_table_hook())) {
        patch_get_map_crc_server.apply();
        if (patch_startup_crc_calc_nop.build(sig_addr1))
            patch_startup_crc_calc_nop.apply();
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
    patch_get_map_crc_server.revert();
}
