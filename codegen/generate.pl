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

# If you see a pull request that changes this system and it does not
# have a change for this constant you should ready the firing squad.
our $VERSION = '1.2.0';

use Digest::SHA1 qw( sha1_base64 );
use File::Basename qw( dirname basename fileparse );
use File::Slurper qw( read_text );
use File::Spec::Functions qw( catfile );
use File::Path qw( make_path );
use YAML::XS qw( LoadFile Load Dump );

use lib dirname(__FILE__); # Include own directory
use CodeGen::Signature qw( yaml_signatures_to_cpp_definitions );

sub gen_header {
    my $name = shift;
qq{/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 *
 * WARNING, THIS FILE WAS AUTOGENERATED. To avoid getting your work overwritten, edit $name instead.
 */

};
}

use constant GEN_INFO_FILE => "./.codegen_hashes.tmp";
# Compile info
my %info = (
    version => $VERSION,
);
# Load info from previous compilation if it exists
if (-e GEN_INFO_FILE) {
    print "[prep] Loading data from previous compilation...";
    # eval to catch it if the GEN_INFO_FILE is corrupt.
    # Insane edge case as no one should ever edit it.
    # But, better safe than sorry.
    eval {
        my $old_info = LoadFile GEN_INFO_FILE;
        if ($old_info->{version} eq $info{version}) {
            print "usable!\n";
            # There is a potential for carrying over arbitrary data here.
            # But not in any dangerous way because of the use case.
            $info{hashes} = $old_info->{hashes};
        } else {
            print "not usable.\n";
        } 1;
    } or print "fail.\n";
}

my $input_file_count = scalar @ARGV;
my $i = 0;

foreach my $filepath (@ARGV) {
    # Terminal output non-sense.
    $i++;
    my $progress = int(($i*100)/$input_file_count);
    printf "[%3d%%] Building CPP files for %s...", $progress, $filepath;

    my ($name, $dir, $suffix) = fileparse $filepath;
    # Remove extension from name.
    $name =~ s/\.\w+$//;

    # Get path without file extension.
    my $output_stem = catfile $dir, $name;

    # Get output directory and make it.
    $dir = catfile catfile $dir, "generated";
    make_path $dir;

    # get names of our output files.
    my $output_src  = catfile $dir, "$name.cpp";
    my $output_head = catfile $dir, "$name.hpp";

    # Load yaml definition as text
    my $file = read_text $filepath;

    # Check if file is the same as the last time
    my $sha = sha1_base64 $file;
    my $old_sha = $info{hashes}{$filepath} // "";
    if ($sha eq $old_sha and -e $output_src and -e $output_head) {
        # Skip if the same and both output files exist.
        print "skipped.\n";
        next;
    }
    # Overwrite old hash.
    $info{hashes}{$filepath} = $sha;

    # Actually load the yaml
    $file = Load $file;

    # Open new source and headers for writing.
    open(OUTPUT_SRC,  ">$output_src");
    open(OUTPUT_HEAD, ">$output_head");

    my $license_header = gen_header basename($filepath);

    print OUTPUT_SRC $license_header, "#include <$output_stem.hpp>\n";
    print OUTPUT_HEAD $license_header, "#pragma once\n";

    # Writes all cpp data to the opened files.
    # TODO: Update this when more types of things get written to these.
    if (exists $file->{signatures}) {
        my $output = yaml_signatures_to_cpp_definitions $name, $file->{signatures};
        print OUTPUT_SRC join "\n", @{$output->{source}->{std_includes}}, "\n";
        print OUTPUT_SRC join "\n", @{$output->{source}->{includes}}, "\n";
        print OUTPUT_SRC $output->{source}->{defs};
        print OUTPUT_SRC $output->{source}->{initializer};

        print OUTPUT_HEAD join "\n", @{$output->{header}->{std_includes}}, "\n";
        print OUTPUT_HEAD join "\n", @{$output->{header}->{includes}}, "\n";
        print OUTPUT_HEAD $output->{header}->{defs};
        print OUTPUT_HEAD $output->{header}->{initializer};
    }

    close(OUTPUT_SRC);
    close(OUTPUT_HEAD);

    print "done.\n";
};

# Save info from this compile to make future compiles quicker. If the
# script fails anywhere before this this info can obviously not be saved.
# That's intentional.
print "[100%] Saving compile info.\n";
open(COMPILE_INFO, ">".GEN_INFO_FILE);
print COMPILE_INFO Dump \%info;
close(COMPILE_INFO);
