cmake_minimum_required(VERSION 2.6)
project(glsldb)

# DEBUG
set(CMAKE_BUILD_TYPE "Maintainer")

# path to custom modules 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# maintainer mode
set( CMAKE_CXX_FLAGS_MAINTAINER "-Wall -Wabi -std=c++11" CACHE STRING
    "Flags used by the C++ compiler during maintainer builds."
    FORCE )
set( CMAKE_C_FLAGS_MAINTAINER "-Wall -pedantic" CACHE STRING
    "Flags used by the C compiler during maintainer builds."
    FORCE )
set( CMAKE_EXE_LINKER_FLAGS_MAINTAINER
    "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
    "Flags used for linking binaries during maintainer builds."
    FORCE )
set( CMAKE_SHARED_LINKER_FLAGS_MAINTAINER
    "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
    "Flags used by the shared libraries linker during maintainer builds."
    FORCE )
mark_as_advanced(
    CMAKE_CXX_FLAGS_MAINTAINER
    CMAKE_C_FLAGS_MAINTAINER
    CMAKE_EXE_LINKER_FLAGS_MAINTAINER
    CMAKE_SHARED_LINKER_FLAGS_MAINTAINER )
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Maintainer."
    FORCE )

# platform checks
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(GLSLDB_LINUX 1)
    add_definitions(-DGLSLDB_LINUX)
    if(CMAKE_SIZEOF_VOID_P MATCHES "8")
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-fPIC" WITH_FPIC)
        if(WITH_FPIC)
            add_definitions(-fPIC)
        endif()
  endif()
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(GLSLDB_OSX 1)
    add_definitions(-DGLSLDB_OSX)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(GLSLDB_WIN32 1)
    add_definitions(-DGLSLDB_WIN32)
endif()

set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(GLSLCompiler)
add_subdirectory(glsldb)