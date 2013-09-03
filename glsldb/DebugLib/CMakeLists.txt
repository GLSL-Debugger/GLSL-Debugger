find_package(DL REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(X11 REQUIRED)
endif()

set(DLSYM_SRC
	libdlsym.c)

set(GLSLDEBUG_SRC
	libglsldebug.c
	streamRecorder.c
	streamRecording.c
	replayFunction.c
	glstate.c
	readback.c
	shader.c
	error.c
	memory.c
	hooks.c
	functionList.c
	queries.c
	preExecution.c
	postExecution.c
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE genGetProcAddressHook.pl
	OUTPUT_FILE getProcAddressHook.inc
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE genFunctionPointerTypes.pl
	OUTPUT_FILE functionPointerTypes.inc
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE genReplayFunc.pl
	OUTPUT_FILE replayFunction.c
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE genFunctionList.pl
	OUTPUT_FILE functionList.c
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE mergeAllowedInBeginEndList.pl
	OUTPUT_FILE functionsAllowedInBeginEnd.pl
)
execute_process(COMMAND perl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	INPUT_FILE genFunctionHooks.pl
	OUTPUT_FILE functionHooks.inc
)

include_directories(${PROJECT_SOURCE_DIR}/glsldb
	${PROJECT_SOURCE_DIR}/glsldb/utils
	${X11_INCLUDE_DIR}
)

if(DEFINED USE_MESA)
    include_directories(${PROJECT_SOURCE_DIR}/mesa-glsl/glslang/Public)
else()
    include_directories(${PROJECT_SOURCE_DIR}/GLSLCompiler/glslang/Public)
endif()

# FIXME
# The makefiles have a target to generate a list of allowed functions between begin/end.
add_library(functionList SHARED functionList.c)

add_library(glsldebug SHARED ${GLSLDEBUG_SRC})
target_link_libraries(glsldebug utils glenumerants ${DL_LIBRARIES})

add_library(dlsym SHARED ${DLSYM_SRC})
target_link_libraries(dlsym ${DL_LIBRARIES})
