#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;

sub preprocess_signature {
    $_->{uc_name} = $_->{uc_name} || uc $_->{name};
    $_->{offset}  = $_->{offset}  || 0;
    $_->{type}    = $_->{type}    || "uintptr_t";
    $_->{multi}   = $_->{multi}   || 0;
    # crucial can be
    if (not exists $_->{crucial}) {
        $_->{crucial} = 0;
    }
    return $_;
}

sub yaml_sig_to_c_sig {
    # Convert string to individual parts and then convert them into C++ format.
    my @bytes = map {$_ eq "??" ? "-1" : "0x$_"} split /\s+/, $_->{bytes};
    # Combine the bytes into a single string.
    my $byte_str = join ", ", @bytes;
    # Get amount of bytes.
    my $len = scalar @bytes;
    return "static LiteSignature sig_$_->{name} = ".
           "{ \"$_->{name}\", $len, { $byte_str } };\n";
}

sub yaml_sig_to_c_initializer {
    if ($_->{multi}) {
        return "    PTRS_$_->{uc_name} = sig_$_->{name}.search_multiple();\n";
    }
    return "    PTR_$_->{uc_name} = sig_$_->{name}.search();\n";
}

sub yaml_sig_to_c_validator {
    my $validator = "";
    if ($_->{multi}) {
        $validator .= "    if (!PTRS_$_->{uc_name}.size())\n";
    } else {
        $validator .= "    if (NOT_FOUND(PTR_$_->{uc_name}))\n";
    }

    if ($_->{crucial}) {
        $validator .= "        crucial_missing.push_back(sig_$_->{name}.name);\n";
    } else {
        $validator .= "        non_crucial_missing.push_back(sig_$_->{name}.name);\n";
    }
    return $validator;
}

sub yaml_sig_to_c_address_var {
    if ($_->{multi} || 0) {
        return "static std::vector<uintptr_t> PTRS_$_->{uc_name};\n";
    }
    return "static uintptr_t PTR_$_->{uc_name};\n";
}

sub yaml_sig_to_c_getter {
    if ($_->{multi}) {
        return "std::vector<uintptr_t> $_->{name}() {\n".
               "    return PTRS_$_->{uc_name};\n".
               "}\n";
    }
    return "$_->{type} $_->{name}() {\n".
           "    return reinterpret_cast<$_->{type}>(\n".
           "        NOT_FOUND(PTR_$_->{uc_name}) ?\n".
           "            0 : (PTR_$_->{uc_name} + $_->{offset}));\n".
           "}\n";

}

sub yaml_sig_to_c_getter_header {
    if ($_->{multi}) {
        return "std::vector<uintptr_t> $_->{name}();\n";
    }
    return "$_->{type} $_->{name}();\n";
}

sub yaml_signatures_to_cpp_definitions {
    my $name = shift;
    my $sigs = shift;
    my @sigs = map { preprocess_signature } @{$sigs};

    #### Source file stuff.
    my $std_includes = [
        "#include <cstdio>",
        "#include <cstdint>",
        "#include <cstdlib>",
        "#include <vector>",
        ];
    my $includes = [
        "#include <hooker/hooker.hpp>",
    ];
    my $source_defs = join "",
        # Actual signature definitions
        (map { yaml_sig_to_c_sig } @sigs), "\n",
        # The variables that hold the addresses.
        (map { yaml_sig_to_c_address_var } @sigs), "\n",
        # Getters for these addresses.
        (map { yaml_sig_to_c_getter } @sigs), "\n";

    my $initialization_code = join "", map { yaml_sig_to_c_initializer } @sigs;
    my $validation_code = join "", map { yaml_sig_to_c_validator } @sigs;

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
    my $header_getters = join "", (map { yaml_sig_to_c_getter_header } @sigs), "\n";
    my $header_initializer = "void init_$name\_signatures();\n";

    return {
        source => {
            std_includes    => $std_includes,
            includes        => $includes,
            defs            => $source_defs,
            initializer     => $init_function,
        },
        header => {
            std_includes    => [],
            includes        => [],
            defs            => $header_getters,
            initializer     => $header_initializer,
        },
    };
}

1;
