find_package(Perl REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(DEBUGLIB_OS_DEPENDENT_LIBS "")
set(DEBUGLIB_OS_DEPENDENT_INCLUDES "")

if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(DL REQUIRED)
	find_package(X11 REQUIRED)
	set(DLSYM_SRC libdlsym.c)
	set(DEBUGLIB_OS_DEPENDENT_LIBS ${DL_LIBRARIES})
	set(DEBUGLIB_OS_DEPENDENT_INCLUDES ${X11_INCLUDE_DIR})
endif()
if(GLSLDB_WIN)
	#add_definitions(/DGLSLDEBUGLIB_HOST)
	set(DEBUGLIB_OS_DEPENDENT_LIBS mhook ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
	set(DEBUGLIB_OS_DEPENDENT_INCLUDES
		"${PROJECT_SOURCE_DIR}/3rdparty/mhook/mhook-lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../GLSLCompiler/glslang/OSDependent/Windows"
	)
endif()


set(GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generator")
set(GENERATOR_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATOR_OUTPUT_DIR}")
set(ENUMERANTS_SRC glenumerants.c)
set(BEGINENDTEST_SRC "${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c")

set(GLSLDEBUG_GEN_SRC
	"${GENERATOR_OUTPUT_DIR}/replayFunction.c"
	"${GENERATOR_OUTPUT_DIR}/functionList.c"
)
set(GLSLDEBUG_OS_SRC "")

if(GLSLDB_WIN)
	set(GLSLDEBUG_OS_SRC initLib.c)
	set(GLSLDEBUG_GEN_SRC "${GLSLDEBUG_GEN_SRC}" "${GENERATOR_OUTPUT_DIR}/glsldebug.def")
	set(CLIENT_SRC debugClient.c)
endif()


set(GLSLDEBUG_SRC
	libglsldebug.c
	streamRecorder.c
	streamRecording.c
	glstate.c
	readback.c
	shader.c
	error.c
	memory.c
	hooks.c
	queries.c
	preExecution.c
	postExecution.c
	${GLSLDEBUG_OS_SRC}
	${GLSLDEBUG_GEN_SRC}
)


# Generate enumerants
message(STATUS "Generate enumerants")
execute_process(COMMAND "${PERL_EXECUTABLE}" Enumerants.pl
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/glenumerants.h"
)

if(GLSLDB_LINUX OR GLSLDB_OSX)
	execute_process(COMMAND "${PERL_EXECUTABLE}" Enumerants.pl -m glx
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/glxenumerants.h"
	)
endif()
if(GLSLDB_WIN)
	execute_process(COMMAND "${PERL_EXECUTABLE}" Enumerants.pl -m wgl
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/wglenumerants.h"
	)
endif()


# Generate generator for list of allowed functions between begin/end
message(STATUS "Generate allowed function list generator.")
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE BeginEndFunctionTest.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c"
)

# Generate debug library
message(STATUS "Generate debug library")
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE GetProcAddressHook.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/getProcAddressHook.inc"
)
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE FunctionPointerTypes.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/functionPointerTypes.inc"
)
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE ReplayFunc.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/replayFunction.c"
)
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE FunctionList.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/functionList.c"
)
execute_process(COMMAND "${PERL_EXECUTABLE}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	INPUT_FILE mergeAllowedInBeginEndList.pl
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm"
)
execute_process(COMMAND "${PERL_EXECUTABLE}" FunctionHooks.pl -p "${GENERATOR_OUTPUT_DIR}"
	WORKING_DIRECTORY "${GENERATOR_DIR}"
	OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/functionHooks.inc"
)

if(GLSLDB_WIN)
	# Generate windows trampolines
	message(STATUS "Generate trampolines")
	execute_process(COMMAND "${PERL_EXECUTABLE}" Trampolines.pl decl
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/trampolines.h"
	)
	execute_process(COMMAND "${PERL_EXECUTABLE}" Trampolines.pl def
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/trampolines.inc"
	)
	execute_process(COMMAND "${PERL_EXECUTABLE}" Trampolines.pl exp
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		OUTPUT_FILE "${GENERATOR_OUTPUT_DIR}/glsldebug.def"
	)
endif()

include_directories(
	"${PROJECT_SOURCE_DIR}/glsldb"
	"${PROJECT_SOURCE_DIR}/glsldb/DebugLib"
	"${PROJECT_SOURCE_DIR}/glsldb/utils"
	"${CMAKE_CURRENT_BINARY_DIR}"
	${DEBUGLIB_OS_DEPENDENT_INCLUDES}
)

add_library(glenumerants STATIC ${ENUMERANTS_SRC})

#add_executable(beginEndFunctionTest ${BEGINENDTEST_SRC})
#target_link_libraries(beginEndFunctionTest ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

add_library(functionList STATIC "${GENERATOR_OUTPUT_DIR}/functionList.c")

add_library(glsldebug SHARED ${GLSLDEBUG_SRC})
target_link_libraries(glsldebug utils glenumerants ${DEBUGLIB_OS_DEPENDENT_LIBS})

if(GLSLDB_LINUX OR GLSLDB_OSX)
	add_library(dlsym SHARED ${DLSYM_SRC})
	target_link_libraries(dlsym ${DL_LIBRARIES})
endif()

if(GLSLDB_WIN)
	add_executable(debugclient ${CLIENT_SRC})
	target_link_libraries(debugclient glsldebug utils)
	install(TARGETS glsldebug #beginEndFunctionTest
		RUNTIME DESTINATION "${DISTRIBUTION_DIRECTORY}" LIBRARY DESTINATION "${DISTRIBUTION_DIRECTORY}")
endif()
