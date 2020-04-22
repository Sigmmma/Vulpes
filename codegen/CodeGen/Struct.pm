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
use List::Util qw( max );
use Data::Dumper qw( Dumper );
use Carp qw( confess );

use CodeGen::Bitfield qw( preprocess_bitfield yaml_bitfield_to_cpp_definition );
use CodeGen::Shared qw( wrap_text ensure_number indent );

use constant STRUCT_CPP_STD_SOURCE_INCLUDES => [];

use constant STRUCT_CPP_SOURCE_INCLUDES => [];

use constant STRUCT_CPP_STD_HEADER_INCLUDES => [
    "#include <cstdint>",
];

use constant STRUCT_CPP_HEADER_INCLUDES => [
    "#include <vulpes/memory/types.hpp>",
];

sub preprocess_struct_member {
    my ($mem, $parent_struct_name) = @_;

    # Names can be left out for padding.
    if (exists $mem->{name}) {
        $mem->{name} =~ s/ +/_/g;
    }

    my $name = $mem->{name} // "<no name>";

    unless (exists $mem->{type}) {
        confess "struct member $name in $parent_struct_name doesn't have a type "
                . Dumper $mem;
    }

    if ($mem->{type} eq "pad" and not exists $mem->{size}) {
        confess "Type pad field in $parent_struct_name doesn't have a size "
                . Dumper $mem;;
    };

    if ((not $mem->{type} eq "pad") and not exists $mem->{name}) {
        confess "Non pad field in $parent_struct_name doesn't have a name "
                 . Dumper $mem;
    };

    $mem->{array_size} = ensure_number $mem->{array_size} // 1;

    return $mem;
}

sub preprocess_struct {
    my ($struct) = @_;

    unless (exists $struct->{name} or exists $struct->{instance_name}) {
        confess ("structs need either a name for their type, or an instance name " . Dumper($struct));
    }

    if ((not exists $struct->{name}) and (exists $struct->{parent})) {
        confess ("Unamed structs can't have a parent type " . Dumper($struct));
    }

    if ((not exists $struct->{name}) and (exists $struct->{size})) {
        confess ("Unamed structs can't have size asserts " . Dumper($struct));
    }

    my $name = $struct->{name} // $struct->{instance_name};

    $struct->{fields} = [map { preprocess_struct_member($_ , $name) } @{$struct->{fields}}];
    if (exists $struct->{size}) {
        $struct->{size} = ensure_number $struct->{size};
    }

    $struct->{array_size} = ensure_number $struct->{array_size} // 1;

    return $struct;
}

sub build_struct_line {
    my ($mem) = @_;

    my $string = '';

    if (exists $mem->{comment}) {
        my $comment = wrap_text (text => "/* $mem->{comment} */", line_len => 72);
        # Indent and append.
        $string .=  indent(text => $comment, indents => 1)."\n";
    }

    if ($mem->{type} eq "pad") {
        $string .= "    PAD($mem->{size});";
    } elsif ($mem->{type} eq "bitfield") {
        $mem->{instance_name} = delete $mem->{name};

        my $output = yaml_bitfield_to_cpp_definition (preprocess_bitfield $mem);
        # Indent and append.
        $string .= indent(text => $output, indents => 1);
    } elsif ($mem->{type} eq "struct") {
        $mem->{instance_name} = $mem->{name};
        delete $mem->{name};
        my $output = yaml_struct_to_cpp_definition (preprocess_struct $mem);
        # Indent and append.
        $string .= indent(text => $output, indents => 1);
    } else {
        # Treating type as a literal C++ type for now.
        $string .= "    $mem->{type} $mem->{name}". ($mem->{array_size} > 1 ? "[$mem->{array_size}];" : ";");
    }

    return $string;
}

sub yaml_struct_to_cpp_definition {
    my ($struct) = @_;

    # Start definition.
    my $string = "struct";
    if (exists $struct->{name}) {
        $string .= " $struct->{name}";
    }

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
    $string .= join("\n", @fields, "}");

    if (exists $struct->{instance_name}) {
        $string .= " $struct->{instance_name}";
        if ($struct->{array_size} > 1) {
            $string .= "[$struct->{array_size}]";
        }
    }

    $string .= ";";

    if (exists $struct->{size}) {
        $string .= " static_assert(sizeof($struct->{name}) == $struct->{size});";
    }

    $string .= "\n";

    return $string;
}

sub yaml_structs_to_cpp_definitions {
    my ($name, $structs) = @_;

    my @structs = map { preprocess_struct($_) } @{$structs};

    my $defs = join("\n", map { yaml_struct_to_cpp_definition($_) } @structs);

    $defs = join("\n", "#pragma pack(push, 1)\n", $defs, "#pragma pack(pop)\n");

    return {
        source => {
            std_includes    => STRUCT_CPP_STD_SOURCE_INCLUDES,
            includes        => STRUCT_CPP_SOURCE_INCLUDES,
            defs            => "",
            initializer     => "",
        },
        header => {
            std_includes    => STRUCT_CPP_STD_HEADER_INCLUDES,
            includes        => STRUCT_CPP_HEADER_INCLUDES,
            defs            => $defs,
            initializer     => "",
        },
    };
}

1;
