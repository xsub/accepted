#!/usr/bin/env bash

# default test install dir (development version)
PREFIX=--prefix=/tmp/accepted-test

# force debug code off by default, this is used to test build before commiting in put.sh
FLAGS=CPPFLAGS='-DDEBUG_ON=0'

# if -d flag is there then force debug code on
if [ "$1" == "-d" ]
then
	FLAGS=CPPFLAGS='-DDEBUG_ON=1'
	PREFIX=--prefix=/tmp/accepted-test-debug/
fi

# remove previous version in this location 
make clean
make uninstall
# rebuild and install
autoreconf --install
./configure $PREFIX $FLAGS
make install

