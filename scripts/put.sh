#!/usr/bin/env bash

git status -uno
read -p "Do yo want to continue the $0? " a

#./scripts/compile.sh && make && (
# make && ln -s src/accepted rejected

 make && ./src/accepted --help && (
ln -s ./src/accepted rejected
./rejected 
if [ $? -ne -0 ];
then
	echo "running rejected with no prams did not return EXIT_SUCCESS (0), aborting..."
	exit
fi
rm -f ./rejected
./scripts/indent.sh

#socat -h > /dev/null && socat EXEC:"./scripts/test.sh",pty GOPEN:__TESTS_RESULTS.log 
#socat -h || ( script /tmp/__TESTS_RESULTS.log; ./scripts/test.sh; echo "" );
#socat -h > /dev/null ||
LANG=C script -c ./scripts/test.sh __TESTS_RESULTS.log -e
tail /tmp/__TESTS_RESULTS.log 
read -p "review test results... commit?"
git commit -a
read -p "Push? "
./scripts/push.sh
) && exit 0

echo "failed to build, will not commit/push"
exit 1
