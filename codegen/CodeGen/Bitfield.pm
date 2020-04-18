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
use List::Util qw{ max };
use Data::Dumper qw{ Dumper };

use CodeGen::Shared qw( wrap_text );

sub preprocess_bitfield_member {
    my ($opt) = @_;

    die "bitfield members need a name"
    unless exists $opt->{name};

    $opt->{name} =~ s/ +/_/g;

    return $opt;
}

sub preprocess_bitfield {
    my ($bitfield) = @_;

    die ("bitfields need either a name for their type, or an instance name " . Dumper($bitfield))
    unless exists $bitfield->{name} or exists $bitfield->{instance_name};

    my $name = exists $bitfield->{name} ? $bitfield->{name} : $bitfield->{instance_name};

    $bitfield->{fields} = [map { preprocess_bitfield_member $_ } @{$bitfield->{fields}}];

    die "enum $name width is not multiple of 8" unless ($bitfield->{width} % 8 == 0);
    $bitfield->{size} = $bitfield->{width} / 8;

    die "enum $name width is not valid width 8/16/32/64" unless (
            $bitfield->{width} == 8 or
            $bitfield->{width} == 16 or
            $bitfield->{width} == 32 or
            $bitfield->{width} == 64);

    # I is so we can increment each enum option
    my $i = 0;
    foreach my $opt (@{$bitfield->{fields}}) {
        $opt->{bit} //= $i;

        $opt->{type} = "uint$bitfield->{width}_t";

        $opt->{mask} = 1 << $opt->{bit};

        $i = $opt->{bit} + 1;
    };

    return $bitfield;
}

sub yaml_bitfield_to_cpp_definition {
    my ($bitfield) = @_;
    # Start definition.
    my $string = "struct";

    if (exists $bitfield->{name}) {
        $string .= " $bitfield->{name}";
    }

    # Start main part.
    $string .= " {\n";

    # Get the length for allignment.
    my $max_type_len  = max (map { length ($_->{type}) } @{$bitfield->{fields}});
    # + 1 because we're accounting for the semicolon.
    my $max_field_len = max (map { length ($_->{name}) } @{$bitfield->{fields}});

    # Turn each option into a set of lines with allignment.
    my @fields;
    my $i = 0;
    foreach my $field (@{$bitfield->{fields}}) {
        # Pad any bits that were skipped.
        if ($i + 1 < $field->{bit}) {
            my $dif = $field->{bit} - ($i + 1);
            push @fields, "    BITPAD(uint$bitfield->{width}_t, $dif);";
        }

        if (exists $field->{comment}) {
            my $comment = wrap_text (text => "/* $field->{comment} */", line_len => 72);
            # Indent by 4 spaces.
            $comment =~ s/^/    /gm;

            push @fields, $comment;
        }

        push @fields, sprintf "    %- ".$max_type_len
        ."s %- ".$max_field_len."s : 1; // 0x%X", $field->{type}, $field->{name}, $field->{mask};

        $i = $field->{bit};
    }

    $string .= join "\n", @fields;

    # Close struct.
    $string .= "\n}";

    if (exists $bitfield->{instance_name}) {
        $string .= " $bitfield->{instance_name}";
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

    my @structs = map { preprocess_bitfield $_ } @{$structs};

    my $defs = join "\n", map { yaml_bitfield_to_cpp_definition $_ } @structs;

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
