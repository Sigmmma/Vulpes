#!/usr/bin/perl

use strict;
use warnings;
use File::Path qw( rmtree );
use Getopt::Std;

sub usage {
    print
    qq{Usage: $0 <options>

    Options:
        -b build. (Default if no args are given.)

        -c clean up the cmake generated and build files.\n\n};
    exit;
}

sub delete_build_files {
    print "Deleting build files.\n";
    unlink glob('*.dll*'), qw(cmake_install.cmake CMakeCache.txt Makefile);
    rmtree "CMakeFiles";
}
sub build {
    print "Building!\n";
    system "cmake -DCMAKE_TOOLCHAIN_FILE=./Toolchain-cross-mingw32-linux.cmake";
    system "make all";
}

if (@ARGV && $ARGV[0] eq "--help"){usage()}

my %options;
getopts("bch", \%options);

if ($options{h} || @ARGV){usage()}

if ($options{c}) {
    delete_build_files;
    if (!$options{b}) {exit;}
}

build;
