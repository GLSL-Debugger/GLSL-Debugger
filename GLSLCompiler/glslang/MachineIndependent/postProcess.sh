#!/bin/bash
dos2unix glslang.y
bison -t -v -d -o generated_glsl_parser.cpp glslang.y
echo "#include \"../../../glsldb/utils/dbgprint.h\"" > generated_glsl_parser_tmp.cpp
perl -ne "s/(fprintf|YYFPRINTF)\s*\([^,]*\s*/dbgPrint\(DBGLVL_COMPILERINFO/;print;" generated_glsl_parser.cpp >> generated_glsl_parser_tmp.cpp
mv generated_glsl_parser_tmp.cpp generated_glsl_parser.cpp
