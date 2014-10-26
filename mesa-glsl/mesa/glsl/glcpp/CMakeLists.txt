
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

mark_as_advanced(YACC_BINARY FLEX_BINARY)
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/glsldb"
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/glcpp-lex.c"
        SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/glcpp-lex.l"
        COMMAND ${FLEX_EXECUTABLE}
        ARGS --nounistd -o"${CMAKE_CURRENT_BINARY_DIR}/glcpp-lex.c"
             "${CMAKE_CURRENT_SOURCE_DIR}/glcpp-lex.l"
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.c" "${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.h"
        SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/glcpp-parse.y"
        COMMAND ${BISON_EXECUTABLE}
        ARGS -v -o "${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.c"
             -p "glcpp_parser_"
             --defines="${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.h"
             "${CMAKE_CURRENT_SOURCE_DIR}/glcpp-parse.y"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/glcpp-lex.c"
)

SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/glcpp-lex.c" GENERATED)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.c" GENERATED)

file(GLOB SRC_C *.c)
file(GLOB SRC_REMOVE glcpp.c)
list(REMOVE_ITEM SRC_C ${SRC_REMOVE})

set(SRC_C ${SRC_C}
	"${CMAKE_CURRENT_BINARY_DIR}/glcpp-lex.c"
	"${CMAKE_CURRENT_BINARY_DIR}/glcpp-parse.c"
)

add_library(glsl_glcpp_lib ${SRC_C})
