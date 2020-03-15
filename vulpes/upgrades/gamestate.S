#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _gamestate_table_new_wrapper
_gamestate_table_new_wrapper:
        movsx eax, bx
        jmp _gamestate_table_new_replacement
