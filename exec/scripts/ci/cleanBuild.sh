#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0
RESULT=""
cd code/*/
make clean
RESULT=$(make)
cd ../../

echo $RESULT

if echo $RESULT | grep "error"
then 
    exit 1
elif echo $RESULT | grep "warning"
then 
    exit 1
else
    exit 0
fi
#EOF