#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;

sub preprocess_signature {
    $_->{uc_name} //= uc $_->{name};
    $_->{offset}  //= 0;
    $_->{type}    //= "uintptr_t";
    $_->{multi}   //= 0;
    $_->{crucial} //= 0;

    $_->{ref} = $_->{base} ? 1 : 0;

    return $_;
}

sub yaml_sig_to_cpp_sig {
    if ($_->{ref}) {
        # References don't contain signatures.
        return "";
    }
    # Validate the string # https://regex101.com/r/3Rrvy9/1
    unless ($_->{bytes} =~ /^\s*(?:(?:\?\?|[[:xdigit:]]{2})\s??\s*)+$/) {
        die "Signature $_->{name} has an invalid byte pattern."
    }
    # Convert string to individual parts and then convert them into C++ format.
    my @bytes = map {$_ eq "??" ? "-1" : "0x$_"} split /\s+/, $_->{bytes};
    # Combine the bytes into a single string.
    my $byte_str = join ", ", @bytes;
    # Get amount of bytes.
    my $len = scalar @bytes;
    return "static LiteSignature signature_$_->{name} = ".
           "{ \"$_->{name}\", $len, { $byte_str } };\n";
}

sub yaml_sig_to_cpp_initializer {
    if ($_->{ref}) {
        # References are initialized when the thing they reference is initialized.
        return "";
    }
    if ($_->{multi}) {
        return "    PTRS_$_->{uc_name} = signature_$_->{name}.search_multiple();\n";
    }
    return "    PTR_$_->{uc_name} = signature_$_->{name}.search();\n";
}

sub yaml_sig_to_cpp_validator {
    if ($_->{ref}) {
        # References don't need validators.
        return "";
    }
    my $validator = "";
    if ($_->{multi}) {
        $validator .= "    if (PTRS_$_->{uc_name}.empty())\n";
    } else {
        $validator .= "    if (!PTR_$_->{uc_name})\n";
    }

    if ($_->{crucial}) {
        $validator .= "        crucial_missing.push_back(signature_$_->{name}.name);\n";
    } else {
        $validator .= "        non_crucial_missing.push_back(signature_$_->{name}.name);\n";
    }
    return $validator;
}

sub yaml_sig_to_cpp_address_var {
    if ($_->{ref}) {
        # References use the address of whatever they reference
        return "";
    }
    if ($_->{multi}) {
        return "static std::vector<uintptr_t> PTRS_$_->{uc_name};\n";
    }
    return "static uintptr_t PTR_$_->{uc_name};\n";
}

sub yaml_sig_to_cpp_getter {
    if ($_->{ref}) {
        return "$_->{type} sig_$_->{name}() {\n".
               "    return reinterpret_cast<$_->{type}>(\n".
               "        sig_$_->{base}\() ?\n".
               "            (reinterpret_cast<uintptr_t>(sig_$_->{base}\()) + $_->{offset}) : NULL);\n".
               "}\n";
    }

    if ($_->{multi}) {
        return "std::vector<uintptr_t> sig_$_->{name}() {\n".
               "    return PTRS_$_->{uc_name};\n".
               "}\n";
    }
    return "$_->{type} sig_$_->{name}() {\n".
           "    return reinterpret_cast<$_->{type}>(\n".
           "        PTR_$_->{uc_name} ?\n".
           "            (PTR_$_->{uc_name} + $_->{offset}) : NULL);\n".
           "}\n";

}

sub yaml_sig_to_cpp_getter_header {
    if ($_->{multi}) {
        return "std::vector<uintptr_t> sig_$_->{name}();\n";
    }
    return "$_->{type} sig_$_->{name}();\n";
}

sub yaml_signatures_to_cpp_definitions {
    my ($name, $sigs) = @_;
    my @sigs = map { preprocess_signature } @{$sigs};

    #### Source file stuff.
    my $std_includes = [
        "#include <cstdio>",
        "#include <cstdint>",
        "#include <cstdlib>",
        "#include <vector>",
        ];
    my $std_header_includes = [
        "#include <cstdint>",
        "#include <vector>",
        ];
    my $includes = [
        "#include <hooker/hooker.hpp>",
    ];
    my $source_defs = join "",
        # Actual signature definitions
        (map { yaml_sig_to_cpp_sig } @sigs), "\n",
        # The variables that hold the addresses.
        (map { yaml_sig_to_cpp_address_var } @sigs), "\n",
        # Getters for these addresses.
        (map { yaml_sig_to_cpp_getter } @sigs), "\n";

    my $initialization_code = join "", map { yaml_sig_to_cpp_initializer } @sigs;
    my $validation_code = join "", map { yaml_sig_to_cpp_validator } @sigs;

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
    my $header_getters = join "", (map { yaml_sig_to_cpp_getter_header } @sigs), "\n";
    my $header_initializer = "void init_$name\_signatures();\n";

    return {
        source => {
            std_includes    => $std_includes,
            includes        => $includes,
            defs            => $source_defs,
            initializer     => $init_function,
        },
        header => {
            std_includes    => $std_header_includes,
            includes        => [],
            defs            => $header_getters,
            initializer     => $header_initializer,
        },
    };
}

1;
