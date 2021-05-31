
find_package(Perl REQUIRED)

set(GENERATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR})

set(GENERATED_FILES "")
function(generate_file script output doc depends)
	add_custom_command(
		OUTPUT  "${output}"
		COMMAND "${PERL_EXECUTABLE}" -I${GENERATOR_DIR} "${script}" ${ARGN} > "${output}"
		DEPENDS "${GENERATOR_DIR}/${script}" "${depends}"
		WORKING_DIRECTORY "${GENERATOR_DIR}"
		COMMENT "${doc}: ${output}"
	)
	set(GENERATED_FILES "${GENERATED_FILES}" "${output}" PARENT_SCOPE)
endfunction()

# Generate enumerants
message(STATUS "Prepare enumerants")
generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/glenumerants.h" "Generate enumerants" "")
if(GLSLDB_LINUX OR GLSLDB_OSX)
	generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/glxenumerants.h" "Generate GLX enumerants" "" -mglx)
endif()
if(GLSLDB_WIN)
	generate_file(Enumerants.pl "${GENERATOR_OUTPUT_DIR}/wglenumerants.h" "Generate WGL enumerants" "" -mwgl)
endif()

# Generate generator for list of allowed functions between begin/end
if(TESTS)
	generate_file(BeginEndFunctionTest.pl "${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c" "Generate allowed function list" "")
endif()
generate_file(mergeAllowedInBeginEndList.pl "${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm" "Generate allowed functions list library" "")

# Generate debug library
message(STATUS "Prepare debug library")
generate_file(GetProcAddressHook.pl "${GENERATOR_OUTPUT_DIR}/getProcAddressHook.inc" "Generate DebugLib" "")
generate_file(FunctionPointerTypes.pl "${GENERATOR_OUTPUT_DIR}/functionPointerTypes.inc" "Generate DebugLib" "")
generate_file(ReplayFunc.pl "${GENERATOR_OUTPUT_DIR}/replayFunction.c" "Generate DebugLib" "")
generate_file(FunctionList.pl "${GENERATOR_OUTPUT_DIR}/functionList.c" "Generate DebugLib" "")
generate_file(FunctionHooks.pl "${GENERATOR_OUTPUT_DIR}/functionHooks.inc" "Generate DebugLib"
	"${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm" -p"${GENERATOR_OUTPUT_DIR}")

if(GLSLDB_WIN)
	# Generate windows trampolines
	message(STATUS "Prepare trampolines")
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/trampolines.h" "trampolines.h" "" -mdecl)
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/trampolines.inc" "trampolines.inc" "" -mdef)
	generate_file(Trampolines.pl "${GENERATOR_OUTPUT_DIR}/glsldebug.def" "glsldebug.def" "" -mexp)
endif()

add_custom_target(generation DEPENDS ${GENERATED_FILES})
