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
    die "bitfields need either a name for their type, or an instance name"
    unless exists $_->{name} or exists $_->{instance_name};

    $_->{fields} = [map { preprocess_bitfield_member } @{$_->{fields}}];

    die "enum width is not multiple of 8" unless ($_->{width} % 8 == 0);
    $_->{size} = $_->{width} / 8;

    my $i = 0;
    foreach my $opt (@{$_->{fields}}) {
        $opt->{bit} //= $i;

        $opt->{type} = "uint$_->{width}_t";

        $opt->{mask} = 1 << $opt->{bit};

        $i = $opt->{bit} + 1;
    };

    return $_;
}

sub yaml_bitfield_to_cpp_definition {
    # Start definition.
    my $string = "struct";

    if (exists $_->{name}) {
        $string .= " $_->{name}";
    }

    # Start main part.
    $string .= " {\n";

    my $bit_thingy = " : 1;";

    # Get the length for allignment.
    my $max_type_len  = max (map { length ($_->{type}) } @{$_->{fields}});
    # + 1 because we're accounting for the semicolon.
    my $max_field_len = max (map { length ($_->{name}) } @{$_->{fields}}) + length $bit_thingy;

    # Turn each option into a set of lines with allignment.
    my @fields;
    my $i = 0;
    foreach my $field (@{$_->{fields}}) {
        # Pad any bits that were skipped.
        if ($i + 1 < $field->{bit}) {
            my $dif = $field->{bit} - ($i + 1);
            push @fields, "    BITPAD(uint$_->{width}_t, $dif);";
        }

        if (exists $field->{comment}) {
            push @fields, sprintf "    /* %s */", $field->{comment};
        }

        push @fields, sprintf "    %- ".$max_type_len
        ."s %- ".$max_field_len."s // 0x%X", $field->{type}, $field->{name}.$bit_thingy, $field->{mask};

        $i = $field->{bit};
    }

    $string .= join "\n", @fields;

    # Close struct.
    $string .= "\n}";

    if (exists $_->{instance_name}) {
        $string .= " $_->{instance_name}";
    }
    $string .= ";";

    $string .= "\n";

    return $string;
}

sub yaml_bitfields_to_cpp_definitions {
    my ($name, $structs) = @_;

    my $std_header_includes = [
        "#include <cstdint>",
        ];

    my $header_includes = [
        "#include <vulpes/memory/types.hpp>",
        ];

    my @structs = map { preprocess_bitfield } @{$structs};

    my $defs = join "\n", map {yaml_bitfield_to_cpp_definition} @structs;

    $defs = qq{#pragma pack(push, 1)

$defs
#pragma pack(pop)
};

    return {
        source => {
            std_includes    => [],
            includes        => [],
            defs            => "",
            initializer     => "",
        },
        header => {
            std_includes    => $std_header_includes,
            includes        => $header_includes,
            defs            => $defs,
            initializer     => "",
        },
    };
}

1;
