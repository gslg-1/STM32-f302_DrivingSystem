#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0

DIR_RES="docs/code"

# Clear old Result
rm -fr $DIR_RES
mkdir $DIR_RES

# Execute Doxygen
doxygen config/doxygen/doxy-conf

echo 'Finished shell script' $0 '.'
#EOF
