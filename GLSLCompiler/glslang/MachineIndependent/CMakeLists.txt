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

bison_target(GLSLParser glslang.y ${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_parser.cpp)
flex_target(GLSLScanner glslang.l ${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp)
add_flex_bison_dependency(GLSLScanner GLSLParser)
#execute_process(COMMAND postProcess)

# FIXME
# modify this for windows support
# make-solution wasn't used on Windows. Hence, Os-dependent stuff was hard coded
# in make/VS-projects.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ 
	${CMAKE_CURRENT_SOURCE_DIR}/../Include 
	${CMAKE_CURRENT_SOURCE_DIR}/../Public 
	${CMAKE_CURRENT_SOURCE_DIR}/../OSDependent/Linux 
	${CMAKE_CURRENT_SOURCE_DIR}/../../compiler
	${PROJECT_SOURCE_DIR}/glsldb
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(glslang STATIC ${SRC} ${BISON_GLSLParser_OUTPUTS})
target_link_libraries(glslang InitializeDll CodeGen DebugVar DebugJump Preprocessor Ossource)
