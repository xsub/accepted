#!/usr/bin/env bash

git status -uno
read -p "Do yo want to continue the $0? " a

# TODO: once --help (or --version) works again change introduce the the binary check after make test
# i.e.:
# make && ./src/accepted --help && (

./scripts/compile.sh && make && (
ln -s src/accepted rejected
./rejected 
if [ $? -ne -0 ];
then
	echo "running rejected with no prams did not return EXIT_SUCCESS (0), aborting..."
	exit
fi
rm -f ./rejected
./scripts/indent.sh
./scripts/test.sh
read -p "review test results... commit?"
git commit -a
read -p "Push? "
./scripts/push.sh
) && exit 0

echo "failed to build, will not commit/push"
exit 1
