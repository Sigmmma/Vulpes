#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;

# Wraps text into lines with the given length. Tries to preserve word boundaries.
# Example:
# wrap_text ( text => $text, line_len => 80 )
#     will output a version of the string where all lines are wrapped to be 80
#     or less chars.
sub wrap_text {
    my %args = @_;
    # The appended space here is a hack, it is to compensate for me
    # not knowing how to match both \s and the end of the string at the same time.
    my @lines = split /(.{1,$args{line_len}})\s+/, $args{text}.' ';

    return join "\n", (grep {length $_} @lines);
}

sub ensure_number {
    my $text = shift @_;
    if ($text =~ /0x[[:xdigit:]]+/) {
        return hex $text;
    }
    return $text;
}

1;
