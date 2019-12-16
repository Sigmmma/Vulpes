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

return 1;
