#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper;
use File::Basename qw( dirname basename );
use List::AllUtils qw( max );
use YAML::XS qw( LoadFile );

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

my $types = LoadFile(dirname(__FILE__)."/basic_types.yaml");
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

sub as_decimal {
    my $value = shift;
    if ($value =~ /(0x[\da-f]+)/i) {
        return hex $value;
    }
    return $value;
}

##########################
# Main starts here!
##########################

my $filename = 'vulpes/memory/signatures.yaml';

my $file = LoadFile($filename);

unless ( $file ) {
    die "Couldn't load file.";
}
=pod
unless(open OUTPUT, "object.hh"){
    die "Unable to open output file.";
}
=cut


my $name = basename $filename;
print
qq{/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 *
 * Warning, this file was autogenerated. To avoid getting your work overwritten, edit $name instead.
 */

#pragma once

};

if (exists $file->{constants}) {

    # Check if all constants have the required fields.
    foreach (@{$file->{constants}}) {
        die ("Couldn't find \"name\" in \n"  . Dumper $_) unless exists $_->{name};
        die ("Couldn't find \"value\" in \n" . Dumper $_) unless exists $_->{value};
        die ("Couldn't find \"type\" in \n"  . Dumper $_) unless exists $_->{type};
    }
    # Map all pieces for easy printing.
    my @text_parts = map { [ $_->{type}, @{process_enum_member $_} ] } @{$file->{constants}};

    # Pad strings for allignment
    pad_strings @text_parts;

    # Write them all out.
    foreach (@text_parts) {
        my $size = scalar @{$_};
        print trim (join " ", "const", @{$_}[0..1], "=", @{$_}[2..$size-1]), ";\n";
    }

    print "\n\n";
}

if (exists $file->{enums}) {

    foreach (@{$file->{enums}}){
        # Create a scoped enum.
        print "enum class ", $_->{name};
        # Set the parent class if needed.
        if (exists $_->{parent}) {
            print " : ", $_->{parent};
        }
        # Open brackets.
        print " {\n";

        # Map all pieces for easy printing.
        my @text_parts = map { process_enum_member } @{$_->{fields}};

        # Pad strings for allignment
        pad_strings @text_parts;

        # Write our fields to the file.
        foreach (@text_parts) {
            my $size = scalar @{$_};
            # If size is over 1 print with equals sign, otherwise do not.
            if ($size > 1){
                print "    ", trim (join " ", @{$_}[0], "=", @{$_}[1..$size-1]), ",\n";
            }else{
                print "    ", (trim @{$_}), ",\n"
            }
        }

        # Close brackets.
        print "};\n\n";
    }

    print "\n";
}

if (exists $file->{structs}) {
    print "#pragma pack(push, 1)\n\n";

    foreach (@{$file->{structs}}){
        # Create a struct.
        print "struct ", $_->{name};
        # Set the parent class if needed.
        if (exists $_->{parent}) {
            print " : public ", $_->{parent};
        }
        # Open brackets.
        print " {\n";

        # Map all pieces for easy printing.
        my @text_parts = map { process_struct_member } @{$_->{fields}};

        pad_struct_strings @text_parts;

        # Merge all separate line pieces into single strings.
        my @lines = map {
            {
                text => (join "", (trim (join " ", @{$_->{text}})), ";"),
                size => $_->{size}
            }
        } @text_parts;

        # Get the longest line length for comment allignment.
        my $line_length = max map { length $_->{text} } @lines;
        # Write our fields to the file.
        my $current_offset = 0;
        foreach (@lines) {
            my $string = rpad $_->{text}, $line_length;
            my $offset_string = (sprintf '0x%X', $current_offset);

            print "    $string // $offset_string\n";

            $current_offset += $_->{size};
        }

        if (exists $_->{size}) {
            # Put a warning here if the struct size does not match what we're expecting.
            my $expected_size = as_decimal $_->{size};
            if ($current_offset != $expected_size) {
                print "    // WARNING: Expected $expected_size, but got $current_offset!\n";
            }
        }

        # Close brackets.
        print "};";
        # If a size was given we want to add a compile time assert to see
        # if the struct is correct when the C++ processor gets to it.
        if (exists $_->{size}) {
            print " static_assert(sizeof($_->{name}) == $_->{size});";

        }
        print "\n\n";
    }
    print "\n#pragma pack(pop)\n\n";
}



if (exists $file->{signatures}) {
    print "#include <hooker/hooker.hpp>\n\n";

    foreach (@{$file->{signatures}}) {
        # The false bool option gets interpreted as an empty string.
        # Work around that.
        my $cruciality = $_->{crucial} ? "true" : "false";

        # Start printing definition.
        print "static Signature($cruciality, sig_$_->{name}, { ";
        # Convert string to individual parts and then convert them into C++ format.
        my @bytes = map {$_ eq "??" ? "-1" : "0x$_"}
                    split /\s+/, $_->{bytes};
        # Print the bytes.
        print join ", ", @bytes;
        # End definition.
        print " };\n\n";
    }
}
