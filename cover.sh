#!/bin/sh

OUTPUT=./Build/coverage.info
COVER_PATH=./Build/cover

rm -r ${COVER_PATH} ${OUTPUT}
lcov --capture --directory . --output-file ${OUTPUT}
genhtml ${OUTPUT} --output-directory ${COVER_PATH}
mozilla ${COVER_PATH}/index.html
