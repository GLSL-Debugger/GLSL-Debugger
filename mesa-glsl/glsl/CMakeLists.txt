
add_subdirectory(glcpp)
add_subdirectory(builtin_compiler)

find_package(PythonLibs 2.6 REQUIRED)
find_program(PYTHON_EXECUTABLE python)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

mark_as_advanced(YACC_BINARY FLEX_BINARY)


set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang.cpp
        OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glslang_tab.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp
        SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/glsl_lexer.ll
        COMMAND ${FLEX_EXECUTABLE}
        ARGS --nounistd -o${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/glsl_lexer.ll
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/glsl_parser.h
        SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/glsl_parser.yy
        COMMAND ${BISON_EXECUTABLE}
        ARGS -v -o ${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp
             -p "_mesa_glsl_"
             --defines=${CMAKE_CURRENT_SOURCE_DIR}/glsl_parser.h
             ${CMAKE_CURRENT_SOURCE_DIR}/glsl_parser.yy
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp
)


add_custom_target(generate_builtins
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/builtins/tools/generate_builtins.py ${EXECUTABLE_OUTPUT_PATH}/glsl_compiler > ${CMAKE_CURRENT_BINARY_DIR}/builtin_function.cpp
    DEPENDS glsl_compiler)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp GENERATED)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/builtin_function.cpp GENERATED)

file(GLOB SRC_CPP *.cpp)
file(GLOB SRC_C *.c)
file(GLOB SRC_CPP_REMOVE main.cpp builtin_stubs.cpp)
list(REMOVE_ITEM SRC_CPP ${SRC_CPP_REMOVE})
set(SRC ${SRC_C} ${SRC_CPP} ${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp)
add_library(glsl_src_lib ${SRC})

add_executable(glsl_compiler main.cpp)
target_link_libraries(glsl_compiler glsl_src_lib glsl_glcpp_lib glsl_builtin_compiler mesa_lib)

add_executable(glsl_glcpp glcpp/glcpp.c)
target_link_libraries(glsl_glcpp glsl_src_lib glsl_glcpp_lib mesa_lib)

add_library(glsl_lib ${CMAKE_CURRENT_BINARY_DIR}/builtin_function.cpp)
add_dependencies(glsl_lib generate_builtins)
target_link_libraries(glsl_lib glsl_src_lib glsl_glcpp_lib mesa_lib)



