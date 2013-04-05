add_subdirectory(preprocessor)

set(SRC
	Gen_glslang.cpp 
	Gen_glslang_tab.cpp 
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

add_custom_command(OUTPUT "Gen_glslang.cpp"
	COMMAND ${FLEX_EXECUTABLE} -oGen_glslang.cpp glslang.l
	DEPENDS "glslang.l"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(OUTPUT "Gen_glslang_tab.cpp"
	COMMAND "./postProcess.sh"
	DEPENDS "glslang.y"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
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
)

add_library(glslang STATIC ${SRC})
target_link_libraries(glslang InitializeDll CodeGen DebugVar DebugJump Preprocessor Ossource)