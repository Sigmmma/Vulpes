#
# This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
# Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
#
# Vulpes is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, version 3.
#
# Vulpes is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
#

use strict;
use warnings;

# Wraps text into lines with the given length. Tries to preserve word boundaries.
# Ex: wrap_text ( text => $text, line_len => 80 )
#     will output a version of the string where all lines are wrapped to be 80
#     or less chars.
sub wrap_text {
    my %args = @_;

    $args{line_len} //= 80;

    # The appended space here is a hack, it is to compensate for me
    # not knowing how to match both \s and the end of the string at the same time.
    my @lines = split /(.{1,$args{line_len}})\s+/, $args{text}.' ';

    return join("\n", (grep {length $_} @lines));
}

sub ensure_number {
    my $text = shift @_;
    if ($text =~ /0x[[:xdigit:]]+/) {
        return hex $text;
    }
    return $text;
}

# args { text => <string>, indents => <number of 4 space indents>, }
sub indent {
    my %args = @_;

    $args{indents} //= 1;

    my $text = $args{text};
    for my $i (1..$args{indents}) {
        $text =~ s/^/    /gm;
    }

    return $text;
}

1;
