#!/usr/bin/env bash

if [ $? -ne 0 ];
then
	echo "accepted uses Bats (Bash Automated Testing System), please install it (apt get install bats on Debian-like)"
	exit 1
else
	bats ./tests/initial-tests.bats
fi
