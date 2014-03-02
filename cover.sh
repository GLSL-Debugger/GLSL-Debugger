#!/bin/sh

CWD=$(pwd)
OUTPUT=./Build/coverage.info
COVER_PATH=./Build/cover

rm -r ${COVER_PATH} ${OUTPUT}
#lcov --initial --zerocounters --directory $CWD
lcov --no-checksum --capture --directory $CWD --output-file ${OUTPUT}
genhtml ${OUTPUT} --output-directory ${COVER_PATH}
mozilla ${COVER_PATH}/index.html
