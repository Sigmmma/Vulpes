#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _saved_game_file_get_path_to_enclosing_directory
_saved_game_file_get_path_to_enclosing_directory:
        mov esi, edx
        jmp [_saved_game_file_get_path_to_enclosing_directory_ptr]
