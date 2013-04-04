find_package(DL REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

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
	OUTPUT_FILE getProcAddressHook.inc )
#execute_process(COMMAND perl genFunctionPointerTypes.pl > functionPointerTypes.inc)
#execute_process(COMMAND perl genReplayFunc.pl > replayFunction.c)
#execute_process(COMMAND perl genFunctionList.pl > functionList.c)
#execute_process(COMMAND perl mergeAllowedInBeginEndList.pl > functionsAllowedInBeginEnd.pl)
#execute_process(COMMAND perl genFunctionHooks.pl > functionHooks.inc)

include_directories(${PROJECT_SOURCE_DIR}/glsldb
	${PROJECT_SOURCE_DIR}/glsldb/utils
)

# FIXME
# The makefiles have a target to generate a list of allowed functions between begin/end. 
add_library(functionList SHARED functionList.c)

add_library(glsldebug SHARED ${GLSLDEBUG_SRC})
target_link_libraries(glsldebug utils glenumerants)

add_library(dlsym SHARED ${DLSYM_SRC})
target_link_libraries(dlsym ${DL_LIBRARIES})
