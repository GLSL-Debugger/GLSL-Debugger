if(GLSLDB_LINUX OR GLSLDB_OSX)
    set(COMPILER_OS_DEPENDENT_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/glslang/OSDependent/Linux")
endif()
if(GLSLDB_WIN)
	set(COMPILER_OS_DEPENDENT_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/glslang/OSDependent/Windows")
endif()

add_subdirectory(compiler)
add_subdirectory(glslang)
