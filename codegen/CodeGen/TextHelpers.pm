#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

use strict;
use warnings;

sub trim { # Takes a string and trims the whitespace from both ends.
    my $s = shift;
    $s =~ s/^\s+|\s+$//g;
    return $s;
};

sub is_number { # Checks if a number is a decimal or hexadecimal number.
    my $thing = shift;
    return $thing =~ /^0[xX][0-9a-fA-F]+|^\d+/ ? 1 : 0;
}

sub lpad { # left pads a string with spaces to match the given size
    return sprintf "%+$_[1]s", $_[0];
}

sub rpad { # right pads a string with spaces to match the given size
    return sprintf "%-$_[1]s", $_[0];
}

sub pad_strings { # Takes an array of arrayrefs and pads every single string
                  # to be the same size as its vertical neighbours.

    # Get the highest number of entries found in our array refs.
    my $width = max map { scalar @{$_} } @_;
    # Pass over the array and for each collumn pad the strings for the correct allignment.
    for my $i (0..$width-1){
        # Get the length of the longest string in this column.
        my $size = max map { length $_->[$i] // 0 } @_;
        # Pad each string in the current column to be the exact same length.
        foreach (@_){
            if (exists $_->[$i]){
                # Check if it is a decimal number or a hexadecimal number
                if (is_number $_->[$i]) {
                    # Numbers should be alligned to the right.
                    $_->[$i] = lpad $_->[$i], $size;
                } else {
                    # Variable references should be alligned to the left.
                    # And should be uppercase, because constants should
                    # only reference constants.
                    $_->[$i] = rpad $_->[$i], $size;
                }
            }
        }
    }
}

sub pad_struct_strings {
    # Get the highest number of entries found in our array refs.
    my $width = max map { scalar @{$_->{text}} } @_;
    # Pass over the array and for each collumn pad the strings for the correct allignment.
    for my $i (0..$width-1){
        # Get the length of the longest string in this column.
        my $size = max map { length $_->{text}->[$i] // 0 } @_;
        # Pad each string in the current column to be the exact same length.
        foreach (@_){
            if (exists $_->{text}->[$i]){
                # Check if it is a decimal number or a hexadecimal number
                if (is_number $_->{text}->[$i]) {
                    # Numbers should be alligned to the right.
                    $_->{text}->[$i] = lpad $_->{text}->[$i], $size;
                } else {
                    # Variable references should be alligned to the left.
                    # And should be uppercase, because constants should
                    # only reference constants.
                    $_->{text}->[$i] = rpad $_->{text}->[$i], $size;
                }
            }
        }
    }
}

sub as_decimal {
    my $value = shift;
    if ($value =~ /(0x[\da-f]+)/i) {
        return hex $value;
    }
    return $value;
}


### TODO: TO BE DEPRICATED

sub process_enum_member { # Returns a listref containing all the parts needed
                          # for an enum member.
    # Split by + sign while keeping + signs.
    my @enum_values = (split /(\+)/, ($_->{value}) // "");
    # upper case the values if they are references
    @enum_values = map { (is_number $_) ? $_ : uc $_ } @enum_values;

    my @enum = ( (uc $_->{name}), @enum_values );
    # Trim the whitespace that we got from our ugly regex split.
    return [map { trim $_ } @enum];
}

my $types = LoadFile catfile(dirname(__FILE__), "basic_types.yaml");
$types = $types->{types};

sub process_struct_member { # Returns a listref containing all the parts needed
                            # for a struct member.

    # Figure out the size that this element would be in memory.
    my $size = $_->{size} // 1;
    $size = ($types->{$_->{type}} // 1) * $size;

    my $text = [""];
    # Padding should under no circumstance be used for anything.
    # Use the macro to make it inaccessible.
    if ($_->{type} eq "pad") {
        $text = [ "PAD($_->{size})" ];
    # Unknowns should under no circumstance be used for anything.
    # Define as padding.
    } elsif ($_->{name} eq "unknown") {
        $text = [ "UNKNOWN", "($_->{type})" ];
    # Handle arrays.
    } elsif (exists $_->{size}) {
        $text = [ $_->{type}, "$_->{name}\[$_->{size}\]" ]
    # Default.
    } else {
        $text = [ $_->{type}, $_->{name} ];
    }
    return {
        text => $text,
        size => $size
    }
}

1;
