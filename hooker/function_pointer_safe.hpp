/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <utility>

template<typename T, typename ... Args>
static inline bool exec_if_valid_bool(T function, Args&& ... args) {
    if(function != NULL) {
        return function(std::forward<Args>(args) ...);
    };
    return false; // Can't have it all :/
}

template<typename T, typename ... Args>
static inline void exec_if_valid(T function, Args&& ... args) {
    if(function != NULL) {
        function(std::forward<Args>(args) ...);
    };
}
