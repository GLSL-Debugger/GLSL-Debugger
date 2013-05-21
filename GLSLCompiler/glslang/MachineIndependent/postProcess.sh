#!/bin/bash
perl -wne 'BEGIN { print "#include \"utils/dbgprint.h\""; } s/(fprintf|YYFPRINTF)\s*\([^,]*\s*/dbgPrint\(DBGLVL_COMPILERINFO/;print;' $1 > $2
