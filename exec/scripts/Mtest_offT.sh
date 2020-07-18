#!/bin/bash
#!!!!!!!!!!!!!!!!!!!!! Don't ferget to set CRLF to LF !!!!!!!!!!!!!!!!!!!!!
echo 'Start shell script '$0

cd tests/MTest/offTar/
make
cd ../../../

echo ""
echo ""
echo ""
echo ""
echo "Result:"
echo ""
./output/offTarMTest/all_tests.exe -v
echo ""
echo ""
echo "Failures:"
echo ""
./output/offTarMTest/all_tests.exe -s
#EOF