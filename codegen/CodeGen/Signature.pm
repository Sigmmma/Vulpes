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

return 1;
