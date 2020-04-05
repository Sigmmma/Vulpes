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

.globl _gamestate_read_from_main_file_hook_wrapper
_gamestate_read_from_main_file_hook_wrapper:
        pushad
        call _gamestate_read_from_main_file_hook
        popad
        mov al, 1
        pop ebx
        pop ecx
        ret

.globl _gamestate_read_from_profile_file_hook_wrapper
_gamestate_read_from_profile_file_hook_wrapper:
        add esp, 0x104
        pushad
        call _gamestate_read_from_profile_file_hook
        popad
        ret

.globl _gamestate_write_to_files_hook_wrapper
_gamestate_write_to_files_hook_wrapper:
        test esi, esi
        setz al
        pushad
        call _gamestate_write_to_files_hook
        popad
        ret

# This hooks into the function that creates backups of savefiles.
# We basically make it execute twice
.globl _gamestate_copy_checkpoint_file_wrapper
_gamestate_copy_checkpoint_file_wrapper:
        mov eax, edi
        mov edx, esi
        jmp _gamestate_copy_checkpoint_file_hook


.globl _gamestate_copy_checkpoint_file
_gamestate_copy_checkpoint_file:
        mov edi, eax
        mov esi, edx
        sub esp, 0x240
        jmp [_gamestate_copy_checkpoint_file_continue_ptr]
