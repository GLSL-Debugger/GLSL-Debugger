add_subdirectory(preprocessor)

if(GLSLDB_WIN AND NOT MINGW)
	add_definitions(/D SH_EXPORTING)
endif()

set(SRC
    "${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_parser.cpp"
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

find_package(BISON 2.7 REQUIRED)
find_package(FLEX REQUIRED)

mark_as_advanced(YACC_BINARY FLEX_BINARY)
bison_target(GLSLParser glslang.y "${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_parser.cpp")
flex_target(GLSLScanner glslang.l "${CMAKE_CURRENT_BINARY_DIR}/generated_glsl_lexer.cpp")
add_flex_bison_dependency(GLSLScanner GLSLParser)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Public"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../compiler"
	${COMPILER_OS_DEPENDENT_INCLUDES}
	"${PROJECT_SOURCE_DIR}/glsldb"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(glslang SHARED ${SRC} ${BISON_GLSLParser_OUTPUTS})
target_link_libraries(glslang utils InitializeDll CodeGen DebugVar DebugJump Preprocessor Ossource)

if(GLSLDB_WIN)
	install(TARGETS glslang RUNTIME DESTINATION "${DISTRIBUTION_DIRECTORY}")
endif()
