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
use Carp qw( confess );

use CodeGen::Shared qw( ensure_number wrap_text );

sub preprocess_enum_option {
    my ($opt) = @_;

    unless (exists $opt->{name}) {
        confess "enum options need a name";
    }

    $opt->{uc_name} = uc $opt->{name};
    # Replace all series of spaces with single underscores
    $opt->{uc_name} =~ s/ +/_/g;

    return $opt;
}

sub preprocess_enum {
    my ($enum) = @_;

    $enum->{options} = [map { preprocess_enum_option $_ } @{$enum->{options}}];

    # This makes it so each option where no value is given the value is 1 above
    # the previous. 0 for the first if not given.
    my $i = 0;
    foreach my $opt (@{$enum->{options}}) {
        $opt->{value} //= $i;
        $opt->{value} = ensure_number $opt->{value};
        $i = $opt->{value} + 1;
    };

    return $enum;
}

sub yaml_enum_to_cpp_definition {
    my ($enum) = @_;

    # Start definition.
    my $string = "enum class $enum->{name}";

    # Optionally specify a base type
    if (exists $enum->{type}) {
        $string .= " : $enum->{type}";
    }

    # Start main part.
    $string .= " {\n";

    # Get the length for allignment.
    my $max_opt_len = max (map { length ($_->{uc_name}) } @{$enum->{options}});

    my $opt_format_str = "    %- ".$max_opt_len."s = %d,";

    # Turn each option into a line with allignment.
    my @options;
    foreach my $opt (@{$enum->{options}}) {
        if (exists $opt->{comment}) {
            my $comment = wrap_text (text => "/* $opt->{comment} */", line_len => 72);
            # Indent by 4 spaces.
            $comment =~ s/^/    /gm;

            push (@options, ($comment));
        }

        push @options, (sprintf ($opt_format_str, ($opt->{uc_name}, $opt->{value})));
    }

    # Close enum.
    $string .= join "\n", @options, "};\n";

    return $string;
}

sub yaml_enums_to_cpp_definitions {
    my ($name, $enums) = @_;

    my $std_header_includes = [
        "#include <cstdint>",
        ];

    my @enums = map { preprocess_enum $_ } @{$enums};

    my $defs = join "\n", map { yaml_enum_to_cpp_definition $_ } @enums;

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
