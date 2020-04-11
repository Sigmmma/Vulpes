#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };

sub preprocess_struct_member {
    $_->{name} =~ s/ +/_/g;

    return $_;
}

sub preprocess_struct {
    $_->{fields} = [map { preprocess_struct_member } @{$_->{fields}}];

    return $_;
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

    # Get the length for allignment.
    my $max_type_len  = max (map { length ($_->{type}) } @{$_->{fields}});
    # + 1 because we're accounting for the semicolon.
    my $max_field_len = max (map { length ($_->{name}) } @{$_->{fields}}) + 1;

    # Turn each option into a line with allignment.
    my @fields = map {
        sprintf "    %- ".$max_type_len
        ."s %- ".$max_field_len."s // 0x", $_->{type}, $_->{name}.";",
    } @{$_->{fields}};

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
            includes        => [],
            defs            => $defs,
            initializer     => "",
        },
    };
}

1;
