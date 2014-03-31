#!/bin/sh

ROOT_PATH=../..
BUILD_PATH=${ROOT_PATH}/Build
OUTPUT=${BUILD_PATH}/coverage.info
COVER_PATH=${BUILD_PATH}/cover

rm -r ${COVER_PATH} ${OUTPUT}
#lcov --initial --zerocounters --directory $CWD
lcov --no-checksum --capture --directory ${ROOT_PATH} --output-file ${OUTPUT}
genhtml ${OUTPUT} --output-directory ${COVER_PATH}
mozilla ${COVER_PATH}/index.html
