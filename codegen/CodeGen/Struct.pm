#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };

sub preprocess_struct_member {
    # Names can be left out for padding.
    if (exists $_->{name}) {
        $_->{name} =~ s/ +/_/g;
    }

    die "struct members need a type" unless exists $_->{type};

    if ($_->{type} eq "pad" and not exists $_->{size}) {die "pad type need a size"};

    return $_;
}

sub preprocess_struct {
    $_->{fields} = [map { preprocess_struct_member } @{$_->{fields}}];

    return $_;
}

sub build_struct_line {
    my $string = '';

    if (exists $_->{comment}) {
        $string .= sprintf "    /* %s */\n", $_->{comment};
    }

    if ($_->{type} eq "pad") {
        $string .= "    PAD($_->{size});";
    } else {
        $string .= sprintf "    $_->{type} $_->{name};";
    }

    return $string;
}

sub yaml_struct_to_cpp_definition {
    # Start definition.
    my $string = "struct $_->{name}";

    # Optionally specify a base type
    if (exists $_->{parent}) {
        $string .= " : $_->{parent}";
    }

    # Start main part.
    $string .= " {\n";

    # TODO: Calculate offsets for more easy crossreference with ASM.
    # The feature from this TODO is currently left out because of time constraints.

    # Turn each option into a line with allignment.
    my @fields = map { build_struct_line } @{$_->{fields}};

    # Close enum.
    $string .= join "\n", @fields, "};";

    if (exists $_->{size}) {
        $string .= " static_assert(sizeof($_->{name}) == $_->{size});";
    }

    $string .= "\n";

    return $string;
}

sub yaml_structs_to_cpp_definitions {
    my ($name, $structs) = @_;

    my $std_header_includes = [
        "#include <cstdint>",
        ];

    my $header_includes = [
        "#include <vulpes/memory/types.hpp>",
        ];

    my @structs = map { preprocess_struct } @{$structs};

    my $defs = join "\n", map {yaml_struct_to_cpp_definition} @structs;

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
