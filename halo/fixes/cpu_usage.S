#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _cpu_usage_sleep_replacement
_cpu_usage_sleep_replacement:
        push 1
        call _Sleep@4
        mov al, BYTE PTR ss:[esp+0x1b]
        ret 4

.att_syntax
