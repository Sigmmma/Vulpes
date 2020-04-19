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
