add_subdirectory(preprocessor)

set(SRC
	${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang_tab.cpp 
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

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang.cpp
	OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glslang_tab.h)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# FIXME
# original makefiles use dos2unix around here too, neccessary?
mark_as_advanced(YACC_BINARY FLEX_BINARY)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang.cpp
	COMMAND ${FLEX_EXECUTABLE} -o${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/glslang.l
	DEPENDS "glslang.l"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.c ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.h
	DEPENDS glslang.y
        COMMAND ${BISON_EXECUTABLE} -t -v -d --file-prefix=${CMAKE_CURRENT_BINARY_DIR}/glslang ${CMAKE_CURRENT_SOURCE_DIR}/glslang.y
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang_tab.cpp
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.c
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/postProcess.sh ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.c ${CMAKE_CURRENT_BINARY_DIR}/Gen_glslang_tab.cpp
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glslang_tab.h
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.h
        COMMAND ${CMAKE_COMMAND} -E
        	copy ${CMAKE_CURRENT_BINARY_DIR}/glslang.tab.h ${CMAKE_CURRENT_BINARY_DIR}/glslang_tab.h
)

#execute_process(COMMAND postProcess)

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
