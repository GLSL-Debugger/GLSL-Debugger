add_subdirectory(preprocessor)

set(SRC
    ${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_parser.cpp
    Initialize.cpp
    IntermTraverse.cpp
    Intermediate.cpp
    ParseHelper.cpp
    PoolAlloc.cpp
    QualifierAlive.cpp
    RemoveTree.cpp
    ShaderLang.cpp
    SymbolTable.cpp
    intermOut.cpp
    parseConst.cpp
    InfoSink.cpp
)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# FIXME
# original makefiles use dos2unix around here too, neccessary?
mark_as_advanced(YACC_BINARY FLEX_BINARY)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp
    COMMAND ${FLEX_EXECUTABLE} -o${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp glslang.l
    DEPENDS "glslang.l"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp_glsl_parser.cpp
    COMMAND ${BISON_EXECUTABLE} -t -v -d -o${CMAKE_CURRENT_BINARY_DIR}/tmp_glsl_parser.cpp glslang.y
    DEPENDS "glslang.y"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(OUTPUT generated_glsl_parser.cpp
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/postProcess.sh tmp_glsl_parser generated_glsl_parser
    DEPENDS "tmp_glsl_parser.cpp"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# FIXME
# modify this for windows support
# make-solution wasn't used on Windows. Hence, Os-dependent stuff was hard coded
# in make/VS-projects.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Public
    ${CMAKE_CURRENT_SOURCE_DIR}/../OSDependent/Linux
    ${CMAKE_CURRENT_SOURCE_DIR}/../../compiler
    ${PROJECT_SOURCE_DIR}/glsldb
)

add_library(glslang STATIC ${SRC})
target_link_libraries(glslang InitializeDll CodeGen DebugVar DebugJump Preprocessor Ossource)
