find_package(DL REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

if(GLSLDB_LINUX OR GLSLDB_OSX)
    find_package(X11 REQUIRED)
endif()


set(GENERATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generator)
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)

set(DLSYM_SRC
    libdlsym.c)

set(ENUMERANTS_SRC glenumerants.c)
set(BEGINENDTEST_SRC ${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c)


set(GLSLDEBUG_GEN_SRC
    ${GENERATOR_OUTPUT_DIR}/replayFunction.c
    ${GENERATOR_OUTPUT_DIR}/functionList.c
)

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
    ${GLSLDEBUG_GEN_SRC}
)


# Generate enumerants
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE Enumerants.pl ../../GL/gl.h ../../GL/glext.h
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/glenumerants.h
)

execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE Enumerants.pl -m glx ../../GL/glx.h ../../GL/glxext.h
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/glxenumerants.h
)

if(WIN32)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE Enumerants.pl -m wgl ../../GL/WinGDI.h ../../GL/wglext.h
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/glxenumerants.h
)
endif()


# Generate generator for list of allowed functions between begin/end
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE BeginEndFunctionTest.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/beginEndFunctionTest.c
)

# Generate debug library
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE GetProcAddressHook.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/getProcAddressHook.inc
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE FunctionPointerTypes.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/functionPointerTypes.inc
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE ReplayFunc.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/replayFunction.c
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE FunctionList.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/functionList.c
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE mergeAllowedInBeginEndList.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/functionsAllowedInBeginEnd.pm
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE FunctionHooks.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/functionHooks.inc
)

include_directories(${PROJECT_SOURCE_DIR}/glsldb
    ${PROJECT_SOURCE_DIR}/glsldb/utils
    ${X11_INCLUDE_DIR}
)


add_library(glenumerants STATIC ${ENUMERANTS_SRC})
add_executable(beginEndFunctionTest ${BEGINENDTEST_SRC})

add_library(functionList SHARED functionList.c)

add_library(glsldebug SHARED ${GLSLDEBUG_SRC})
target_link_libraries(glsldebug utils glenumerants ${DL_LIBRARIES})

add_library(dlsym SHARED ${DLSYM_SRC})
target_link_libraries(dlsym ${DL_LIBRARIES})
