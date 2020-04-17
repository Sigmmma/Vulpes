#
# Vulpes (c) 2020 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;
use List::Util qw{ max };
use CodeGen::Shared qw{ ensure_number wrap_text };

sub preprocess_enum_option {
    die "enum options need a name"
    unless exists $_->{name};

    $_->{uc_name} = uc $_->{name};
    # Replace all series of spaces with single underscores
    $_->{uc_name} =~ s/ +/_/g;

    return $_;
}

sub preprocess_enum {
    $_->{options} = [map { preprocess_enum_option } @{$_->{options}}];

    # Make it so each value that isn't given is the previous one incremented.
    my $i = 0;
    foreach my $opt (@{$_->{options}}) {
        $opt->{value} //= $i;
        $opt->{value} = ensure_number $opt->{value};
        $i = $opt->{value} + 1;
    };

    return $_;
}

sub yaml_enum_to_cpp_definition {
    # Start definition.
    my $string = "enum class $_->{name}";

    # Optionally specify a base type
    if (exists $_->{type}) {
        $string .= " : $_->{type}";
    }

    # Start main part.
    $string .= " {\n";

    # Get the length for allignment.
    my $max_opt_len = max (map { length ($_->{uc_name}) } @{$_->{options}});

    # Turn each option into a line with allignment.
    my @options;
    foreach my $opt (@{$_->{options}}) {
        if (exists $opt->{comment}) {
            my $comment = wrap_text (text => "/* $opt->{comment} */", line_len => 72);
            # Indent by 4 spaces.
            $comment =~ s/^/    /gm;

            push @options, $comment;
        }

        push @options, sprintf "    %- ".$max_opt_len."s = %d,", $opt->{uc_name}, $opt->{value};
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

    my @enums = map { preprocess_enum } @{$enums};

    my $defs = join "\n", map {yaml_enum_to_cpp_definition} @enums;

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
