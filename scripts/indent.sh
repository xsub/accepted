#!/bin/bash
# accepted/rejected formatter we want clang-format, if not use indent
FILES="src/main.c"
TOOL="clang-format"
TOOL_OPT="-i"

if [ ! "$TOOL --version" ]
then
    TOOL="indent"
    TOOL_OPT=""
fi

echo "using $TOOL; Files: $FILES"

$TOOL $TOOL_OPT $FILES 
