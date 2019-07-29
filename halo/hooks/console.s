#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _console_hook__console_in
_console_hook__console_in:
        push edi
        call _handler___process_command
        pop edi

        cmp al, 0
        jne continue_to_halo_con_in
        xor al, al
        add esp, 0x500
        ret
    continue_to_halo_con_in:
        jmp [_console_hook__return_to_halo_con_in]

.globl _console_hook__auto_complete
_console_hook__auto_complete:
        mov edx, [_console_hook__results_ptr]
        mov edx, [edx]
        # Shove all of this data into our autocomplete function
        push [_console_hook__count_ptr]
        push edx
        call _handler__auto_complete
        add esp, 8
        # move the output count to where the code expects it to be
        mov edx, [_console_hook__count_ptr]
        movsx edx, WORD PTR ds:[edx]
        ret

.att_syntax
