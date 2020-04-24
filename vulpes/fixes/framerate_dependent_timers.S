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

.intel_syntax noprefix

.globl _update_contrails_wrapper
_update_contrails_wrapper:
        mov esi, 0 // Not sure why this should be 0, but it works
        push 0x3d088889 // 0.0333333f
                        // Make the contrail function think exactly a tick
                        // passed since the last call.
                        // Since this is now called on tick anyway.
        call [_contrails_update_func]
        pop esi
        ret

.att_syntax
