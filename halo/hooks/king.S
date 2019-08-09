#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _king_timer_reset_hook
_king_timer_reset_hook:
        pushad
        call _king_timer_reset
        popad
        ret

.att_syntax
