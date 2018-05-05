#!/bin/bash
# accepted/rejected formatter we want clang-format, if not use indent
FILES="src/main.c"
TOOL="clang-format"
TOOL_OPT="-i"

which $TOOL
if [ $? -ne 0 ]
then
    TOOL="indent"
    TOOL_OPT=""
fi

echo "Format status (cloc), before:"
cloc $FILES | grep -v 'http://' | tee /tmp/$$.1

echo "using $TOOL; Files: $FILES"

$TOOL $TOOL_OPT $FILES 

echo "Format status (cloc), after:"
cloc $FILES | grep -v 'http://' | tee /tmp/$$.2

#sed -i '$ d' /tmp/$$.*

echo "Format change:"
diff -W160 -y --suppress-common-lines /tmp/$$.*


rm -f /tmp/$$.*
