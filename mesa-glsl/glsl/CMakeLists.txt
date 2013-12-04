
add_subdirectory(glcpp)
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/mesa-glsl
	${PROJECT_SOURCE_DIR}/mesa-glsl/mapi
	${PROJECT_SOURCE_DIR}/mesa-glsl/mesa
	${PROJECT_SOURCE_DIR}/mesa-glsl/glsl
	${PROJECT_SOURCE_DIR}/mesa-glsl/glsl/glcpp
)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

mark_as_advanced(YACC_BINARY FLEX_BINARY)

execute_process(COMMAND ${FLEX_EXECUTABLE} --nounistd -o${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp glsl_lexer.ll
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

execute_process(COMMAND ${BISON_EXECUTABLE} -v -o ${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp
             -p "_mesa_glsl_" --defines=${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.h glsl_parser.yy
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp GENERATED)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.h GENERATED)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp GENERATED)

file(GLOB SRC_CPP *.cpp)
file(GLOB SRC_C *.c)
file(GLOB SRC_CPP_REMOVE main.cpp)
list(REMOVE_ITEM SRC_CPP ${SRC_CPP_REMOVE})
set(SRC ${SRC_C} ${SRC_CPP}
		${CMAKE_CURRENT_BINARY_DIR}/glsl_lexer.cpp
		${CMAKE_CURRENT_BINARY_DIR}/glsl_parser.cpp)
add_library(glsl_lib ${SRC})
target_link_libraries(glsl_lib glsl_glcpp_lib mesa_lib)

add_executable(glsl_compiler main.cpp)
target_link_libraries(glsl_compiler glsl_lib)

add_executable(glsl_glcpp glcpp/glcpp.c)
target_link_libraries(glsl_glcpp glsl_lib)
