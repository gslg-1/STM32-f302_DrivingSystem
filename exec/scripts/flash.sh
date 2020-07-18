#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0

ST-LINK_CLI -c -ME && \
ST-LINK_CLI -c -P ./output/build/*.hex

#EOF