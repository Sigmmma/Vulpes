#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };

use CodeGen::Bitfield qw( preprocess_bitfield yaml_bitfield_to_cpp_definition );
use CodeGen::Shared qw( wrap_text ensure_number );

sub preprocess_struct_member {
    # Names can be left out for padding.
    if (exists $_->{name}) {
        $_->{name} =~ s/ +/_/g;
    }

    my $name = exists $_->{name} ? $_->{name} : "<no name>";

    die "struct member $name doesn't have a type" unless exists $_->{type};

    if ($_->{type} eq "pad" and not exists $_->{size}) {die "pad type need a size"};

    $_->{array_size} //= 1;

    return $_;
}

sub preprocess_struct {
    $_->{fields} = [map { preprocess_struct_member } @{$_->{fields}}];
    if (exists $_->{size}) {
        $_->{size} = ensure_number $_->{size};
    }

    return $_;
}

sub build_struct_line {
    my $string = '';

    if (exists $_->{comment}) {
        my $comment = wrap_text (text => "/* $_->{comment} */", line_len => 72);
        # Indent by 4 spaces.
        $comment =~ s/^/    /gm;

        $string .= "$comment\n";
    }

    if ($_->{type} eq "pad") {
        $string .= "    PAD($_->{size});";
    } elsif ($_->{type} eq "bitfield") {
        $_->{instance_name} = $_->{name};
        delete $_->{name};
        my $output = yaml_bitfield_to_cpp_definition (preprocess_bitfield $_);
        # Add the indent.
        $output =~ s/^/    /gm;

        $string .= $output;
    } else {
        $string .= "    $_->{type} $_->{name}". ($_->{array_size} > 1 ? "[$_->{array_size}];" : ";");
    }

    return $string;
}

sub yaml_struct_to_cpp_definition {
    # Start definition.
    my $string = "struct $_->{name}";

    # Optionally specify a base type
    if (exists $_->{parent}) {
        $string .= " : public $_->{parent}";
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
