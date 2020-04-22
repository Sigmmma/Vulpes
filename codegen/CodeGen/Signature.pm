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
use Data::Dumper qw( Dumper );
use Carp qw( confess );

use CodeGen::Shared qw( ensure_number );

use constant SIGNATURE_CPP_STD_SOURCE_INCLUDES => [
    "#include <cstdio>",
    "#include <cstdint>",
    "#include <cstdlib>",
    "#include <vector>",
];

use constant SIGNATURE_CPP_SOURCE_INCLUDES => [
    "#include <hooker/hooker.hpp>",
];

use constant SIGNATURE_CPP_STD_HEADER_INCLUDES => [
    "#include <cstdint>",
    "#include <vector>",
];

use constant SIGNATURE_CPP_HEADER_INCLUDES => [];

sub preprocess_signature {
    my ($sig) = @_;

    unless (exists $sig->{name}) {
        confess "Signatures need a name " . Dumper $sig;
    }

    $sig->{uc_name}   = uc $sig->{name};
    $sig->{offset}    = ensure_number($sig->{offset} // 0);
    $sig->{type}    //= "uintptr_t";
    $sig->{multi}   //= 0;
    $sig->{crucial} //= 0;

    $sig->{ref} = !!$sig->{base};

    return $sig;
}

sub yaml_sig_to_cpp_sig {
    my ($sig) = @_;

    if ($sig->{ref}) {
        # References don't contain signatures.
        return "";
    }
    # Validate the string # https://regex101.com/r/3Rrvy9/1
    unless ($sig->{bytes} =~ /^\s*(?:(?:\?\?|[[:xdigit:]]{2})\s??\s*)+$/) {
        confess "Signature $sig->{name} has an invalid byte pattern."
    }
    # Convert string to individual parts and then convert them into C++ format.
    my @bytes = map {$_ eq "??" ? "-1" : "0x$_"} split /\s+/, $sig->{bytes};
    # Combine the bytes into a single string.
    my $byte_str = join(", ", @bytes);
    # Get amount of bytes.
    my $len = scalar @bytes;
    return "static LiteSignature signature_$sig->{name} = ".
           "{ \"$sig->{name}\", $len, { $byte_str } };\n";
}

sub yaml_sig_to_cpp_initializer {
    my ($sig) = @_;

    if ($sig->{ref}) {
        # References are initialized when the thing they reference is initialized.
        return "";
    }
    if ($sig->{multi}) {
        return "    PTRS_$sig->{uc_name} = signature_$sig->{name}.search_multiple();\n";
    }
    return "    PTR_$sig->{uc_name} = signature_$sig->{name}.search();\n";
}

sub yaml_sig_to_cpp_validator {
    my ($sig) = @_;

    if ($sig->{ref}) {
        # References don't need validators.
        return "";
    }
    my $validator = "";
    if ($sig->{multi}) {
        $validator .= "    if (PTRS_$sig->{uc_name}.empty())\n";
    } else {
        $validator .= "    if (!PTR_$sig->{uc_name})\n";
    }

    if ($sig->{crucial}) {
        $validator .= "        crucial_missing.push_back(signature_$sig->{name}.name);\n";
    } else {
        $validator .= "        non_crucial_missing.push_back(signature_$sig->{name}.name);\n";
    }
    return $validator;
}

sub yaml_sig_to_cpp_address_var {
    my ($sig) = @_;

    if ($sig->{ref}) {
        # References use the address of whatever they reference
        return "";
    }
    if ($sig->{multi}) {
        return "static std::vector<uintptr_t> PTRS_$sig->{uc_name};\n";
    }
    return "static uintptr_t PTR_$sig->{uc_name};\n";
}

sub yaml_sig_to_cpp_getter {
    my ($sig) = @_;

    if ($sig->{ref}) {
        return "$sig->{type} sig_$sig->{name}() {\n".
               "    return reinterpret_cast<$sig->{type}>(\n".
               "        sig_$sig->{base}\() ?\n".
               "            (reinterpret_cast<uintptr_t>(sig_$sig->{base}\()) + $sig->{offset}) : NULL);\n".
               "}\n";
    }

    if ($sig->{multi}) {
        return "std::vector<uintptr_t> sig_$sig->{name}() {\n".
               "    return PTRS_$sig->{uc_name};\n".
               "}\n";
    }
    return "$sig->{type} sig_$sig->{name}() {\n".
           "    return reinterpret_cast<$sig->{type}>(\n".
           "        PTR_$sig->{uc_name} ?\n".
           "            (PTR_$sig->{uc_name} + $sig->{offset}) : NULL);\n".
           "}\n";

}

sub yaml_sig_to_cpp_getter_header {
    my ($sig) = @_;

    if ($sig->{multi}) {
        return "std::vector<uintptr_t> sig_$sig->{name}();\n";
    }
    return "$sig->{type} sig_$sig->{name}();\n";
}

sub yaml_signatures_to_cpp_definitions {
    my ($name, $sigs) = @_;
    my @sigs = map { preprocess_signature $_ } @{$sigs};

    my $source_defs = join("",
        # Actual signature definitions
        (map { yaml_sig_to_cpp_sig $_ } @sigs), "\n",
        # The variables that hold the addresses.
        (map { yaml_sig_to_cpp_address_var $_ } @sigs), "\n",
        # Getters for these addresses.
        (map { yaml_sig_to_cpp_getter $_ } @sigs), "\n");

    my $initialization_code = join("", map { yaml_sig_to_cpp_initializer $_ } @sigs);
    my $validation_code = join("", map { yaml_sig_to_cpp_validator $_ } @sigs);

    # Function called on initialization.
    my $init_function = qq{void init_$name\_signatures() {
    // Write signature results to our variables for safekeeping.
$initialization_code

    // Find out if we failed to find any signatures.
    std::vector<const char*> crucial_missing;
    std::vector<const char*> non_crucial_missing;

$validation_code

    // Report to the user that we are missing signatures if we are.
    if (crucial_missing.size()) {
        printf("Vulpes connot find the following crucial signatures:\\n");
        for (size_t i=0;i<crucial_missing.size();i++) {
            printf("\%s\\n", crucial_missing[i]);
        };
        if (non_crucial_missing.size()) printf("And less importantly, ");
    }
    if (non_crucial_missing.size()) {
        printf("Vulpes connot find the following non-crucial signatures:\\n");
        for (size_t i=0;i<non_crucial_missing.size();i++) {
            printf("\%s\\n", non_crucial_missing[i]);
        };
    }
    // Close the program if we missed any crucial signatures.
    if (crucial_missing.size()) {
        printf("\\n"
        "Since there is crucial signatures that we cannot find Vulpes "
        "cannot continue loading.\\n"
        "Please submit your error log to the Github issues page.\\n");
        exit(0);
    }
}
};

    #### Header stuff
    my $header_getters = join("", (map { yaml_sig_to_cpp_getter_header($_) } @sigs), "\n");
    my $header_initializer = "void init_$name\_signatures();\n";

    return {
        source => {
            std_includes    => SIGNATURE_CPP_STD_SOURCE_INCLUDES,
            includes        => SIGNATURE_CPP_SOURCE_INCLUDES,
            defs            => $source_defs,
            initializer     => $init_function,
        },
        header => {
            std_includes    => SIGNATURE_CPP_STD_HEADER_INCLUDES,
            includes        => SIGNATURE_CPP_HEADER_INCLUDES,
            defs            => $header_getters,
            initializer     => $header_initializer,
        },
    };
}

1;
