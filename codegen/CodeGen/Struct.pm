#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };
use Data::Dumper qw{ Dumper };

use CodeGen::Bitfield qw( preprocess_bitfield yaml_bitfield_to_cpp_definition );
use CodeGen::Shared qw( wrap_text ensure_number );

sub preprocess_struct_member {
    my ($mem) = @_;

    # Names can be left out for padding.
    if (exists $mem->{name}) {
        $mem->{name} =~ s/ +/_/g;
    }

    my $name = exists $mem->{name} ? $mem->{name} : "<no name>";

    die "struct member $name doesn't have a type" . Dumper $mem
    unless exists $mem->{type};

    if ($mem->{type} eq "pad" and not exists $mem->{size}) {die "pad type need a size"};

    $mem->{array_size} //= 1;

    return $mem;
}

sub preprocess_struct {
    my ($struct) = @_;

    $struct->{fields} = [map { preprocess_struct_member $_ } @{$struct->{fields}}];
    if (exists $struct->{size}) {
        $struct->{size} = ensure_number $struct->{size};
    }

    return $struct;
}

sub build_struct_line {
    my ($mem) = @_;

    my $string = '';

    if (exists $mem->{comment}) {
        my $comment = wrap_text (text => "/* $mem->{comment} */", line_len => 72);
        # Indent by 4 spaces.
        $comment =~ s/^/    /gm;

        $string .= "$comment\n";
    }

    if ($mem->{type} eq "pad") {
        $string .= "    PAD($mem->{size});";
    } elsif ($mem->{type} eq "bitfield") {
        $mem->{instance_name} = $mem->{name};
        delete $mem->{name};
        my $output = yaml_bitfield_to_cpp_definition (preprocess_bitfield $mem);
        # Add the indent.
        $output =~ s/^/    /gm;

        $string .= $output;
    } else {
        $string .= "    $mem->{type} $mem->{name}". ($mem->{array_size} > 1 ? "[$mem->{array_size}];" : ";");
    }

    return $string;
}

sub yaml_struct_to_cpp_definition {
    my ($struct) = @_;

    # Start definition.
    my $string = "struct $struct->{name}";

    # Optionally specify a base type
    if (exists $struct->{parent}) {
        $string .= " : public $struct->{parent}";
    }

    # Start main part.
    $string .= " {\n";

    # TODO: Calculate offsets for more easy crossreference with ASM.
    # The feature from this TODO is currently left out because of time constraints.

    # Turn each option into a line with allignment.
    my @fields = map { build_struct_line $_ } @{$struct->{fields}};

    # Close enum.
    $string .= join "\n", @fields, "};";

    if (exists $struct->{size}) {
        $string .= " static_assert(sizeof($struct->{name}) == $struct->{size});";
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

    my @structs = map { preprocess_struct $_ } @{$structs};

    my $defs = join "\n", map { yaml_struct_to_cpp_definition $_ } @structs;

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
