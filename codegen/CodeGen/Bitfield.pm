#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };

sub preprocess_bitfield_member {
    $_->{name} =~ s/ +/_/g;

    return $_;
}

sub preprocess_bitfield {
    $_->{fields} = [map { preprocess_bitfield_member } @{$_->{fields}}];

    die "enum width is not multiple of 8" unless ($_->{width} % 8 == 0);
    $_->{size} = $_->{width} / 8;

    my $i = 0;
    foreach my $opt (@{$_->{fields}}) {
        $opt->{bit} //= $i;

        if ($_->{width} == 8) {
            $opt->{type} = "uint8_t";
        } elsif ($_->{width} == 16) {
            $opt->{type} = "uint16_t";
        } elsif ($_->{width} == 24) {
            if ($opt->{bit} < 16) {
                $opt->{type} = "uint16_t";
            } else {
                $opt->{type} = "uint8_t";
            }
        } elsif ($_->{width} == 32) {
            $opt->{type} = "uint32_t";
        }

        $opt->{mask} = 1 << $opt->{bit};

        $i = $opt->{bit} + 1;
    };

    return $_;
}

sub yaml_bitfield_to_cpp_definition {
    # Start definition.
    my $string = "struct $_->{name}";

    # Start main part.
    $string .= " {\n";

    my $bit_thingy = " : 1;";

    # Get the length for allignment.
    my $max_type_len  = max (map { length ($_->{type}) } @{$_->{fields}});
    # + 1 because we're accounting for the semicolon.
    my $max_field_len = max (map { length ($_->{name}) } @{$_->{fields}}) + length $bit_thingy;

    # Turn each option into a line with allignment.
    my @fields = map {
        sprintf "    %- ".$max_type_len
        ."s %- ".$max_field_len."s // 0x%X", $_->{type}, $_->{name}.$bit_thingy, $_->{mask}
    } @{$_->{fields}};

    # Close enum.
    $string .= join "\n", @fields, "};";

    if (exists $_->{size}) {
        $string .= " static_assert(sizeof($_->{name}) == $_->{size});";
    }

    $string .= "\n";

    return $string;
}

sub yaml_bitfields_to_cpp_definitions {
    my ($name, $structs) = @_;

    my $std_header_includes = [
        "#include <cstdint>",
        ];

    my @structs = map { preprocess_bitfield } @{$structs};

    my $defs = join "\n", map {yaml_bitfield_to_cpp_definition} @structs;

    $defs = qq{#pragma pack(push, 1)

$defs
#pragma pack(pop)
};
    print $defs;

    return {
        source => {
            std_includes    => [],
            includes        => [],
            defs            => "",
            initializer     => "",
        },
        header => {
            std_includes    => $std_header_includes,
            includes        => [],
            defs            => $defs,
            initializer     => "",
        },
    };
}

1;
