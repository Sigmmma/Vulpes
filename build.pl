#!/usr/bin/perl

use strict;
use warnings;
use File::Path qw( rmtree );
use File::pushd qw( pushd );
use File::Copy qw( copy );
use Getopt::Std;
use YAML::XS qw( LoadFile );

my $config = LoadFile("build_config.yaml");

sub usage {
    print
    qq{Usage: $0 <options>

    Options:
        -b build. (Default if no args are given.)

        -c clean up the cmake generated and build files.

        -j do whatever we're doing for the LuaJIT library too.\n\n};
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
sub delete_luajit_build_files {
    print "Deleting LuaJIT build files.\n";
    unlink
        glob('./lib/LuaJIT/src/*.a'),
        glob('./lib/LuaJIT/src/*.o'),
        glob('./lib/LuaJIT/src/host/*.o'),
        './lib/LuaJIT/src/host/minilua',
        './lib/LuaJIT/src/host/buildvm';
}
sub build_luajit {
    print "Building LuaJIT!\n";
    copy("./lib/LuaJIT_Makefile", "./lib/LuaJIT/src/Makefile");
    {
        my $wd = pushd "./lib/LuaJIT/src";
        system "make HOST_CC=\"gcc -m32\" CROSS=i686-w64-mingw32- TARGET_SYS=Windows";
    }
}

if (@ARGV && $ARGV[0] eq "--help"){usage()}

my %options;
getopts("bchj", \%options);

if ($options{h} || @ARGV){usage()}

if ($options{c}) {
    if ($options{j}){delete_luajit_build_files;}
    delete_build_files;
    if (!$options{b}) {exit;}
}

if ($options{j}){
    build_luajit;
}

build;

if ($config->{game_folder}) {
    printf "Installing Vulpes to %s\n", $config->{game_folder};
    copy("./Vulpes.dll", $config->{game_folder}."/Vulpes.dll");
    if ($config->{strings_build}) {
        print "Using strings.dll configuration\n";
        copy("./VulpesLoader.dll", $config->{game_folder}."/strings.dll");
    } else {
        print "Using controls/ configuration\n";
        copy("./VulpesLoader.dll", $config->{game_folder}."/controls/VulpesLoader.dll");
    }
}
