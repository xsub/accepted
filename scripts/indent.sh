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

echo "using $TOOL; Files: $FILES"

$TOOL $TOOL_OPT $FILES 
