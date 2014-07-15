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

function (generate_file script output doc depends)
	add_custom_command(
		OUTPUT  "${output}"
		COMMAND "${PERL_EXECUTABLE}" "${script}" ${ARGN} > "${output}"
		DEPENDS "${GENERATOR_DIR}/${script}" ${input}
		WORKING_DIRECTORY
			"${GENERATOR_DIR}"
		COMMENT "${doc}"
	)
endfunction ()

# Generate enumerants
generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/glenumerants.h" "Generate enumerants" "")

if(GLSLDB_LINUX OR GLSLDB_OSX)
	generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/glxenumerants.h" "Generate GLX enumerants" "" -m glx)
endif()
if(GLSLDB_WIN)
	generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/wglenumerants.h" "Generate WGL enumerants" "" -m wgl)
endif()


# Generate generator for list of allowed functions between begin/end
generate_file(BeginEndFunctionTest.pl "${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c" "Generate allowed function list" "")

# Generate debug library
generate_file(GetProcAddressHook.pl "${GENERATOR_OUTPUT_DIR}/getProcAddressHook.inc" "Generate debug library: getProcAddressHook.inc" "")
generate_file(FunctionPointerTypes.pl "${GENERATOR_OUTPUT_DIR}/functionPointerTypes.inc" "Generate debug library: functionPointerTypes.inc" "")
generate_file(ReplayFunc.pl "${GENERATOR_OUTPUT_DIR}/replayFunction.c" "Generate debug library: replayFunction.c" "")
generate_file(FunctionList.pl "${GENERATOR_OUTPUT_DIR}/functionList.c" "Generate debug library: functionList.c" "")
generate_file(mergeAllowedInBeginEndList.pl "${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm" "Generate debug library: functionsAllowedInBeginEnd.pm" "")
generate_file(FunctionHooks.pl "${GENERATOR_OUTPUT_DIR}/functionHooks.inc" "Generate debug library: functionHooks.inc" "${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm" -p "${GENERATOR_OUTPUT_DIR}")

if(GLSLDB_WIN)
	# Generate windows trampolines
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/trampolines.h" "Generate trampolines: trampolines.h" "" decl)
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/trampolines.inc" "Generate trampolines: trampolines.inc" "" def)
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/glsldebug.def" "Generate trampolines: glsldebug.def" "" exp)
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
