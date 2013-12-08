#!/bin/bash
#dos2unix glslang.y
perl -wne 'BEGIN { print "#include \"utils/dbgprint.h\"\n"; } s/(fprintf|YYFPRINTF)\s*\([^,]*\s*/dbgPrint\(DBGLVL_COMPILERINFO/;print;' $1.cpp > $2.cpp
cp $1.hpp $2.hpp

