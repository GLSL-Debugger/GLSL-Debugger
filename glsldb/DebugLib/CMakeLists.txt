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

set(GENERATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generator)
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)

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
    INPUT_FILE genReplayFunc.pl
    OUTPUT_FILE ${GENERATOR_OUTPUT_DIR}/replayFunction.c
)
execute_process(COMMAND perl
    WORKING_DIRECTORY ${GENERATOR_DIR}
    INPUT_FILE genFunctionList.pl
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

# FIXME
# The makefiles have a target to generate a list of allowed functions between begin/end.
add_library(functionList SHARED functionList.c)

add_library(glsldebug SHARED ${GLSLDEBUG_SRC})
target_link_libraries(glsldebug utils glenumerants ${DL_LIBRARIES})

add_library(dlsym SHARED ${DLSYM_SRC})
target_link_libraries(dlsym ${DL_LIBRARIES})
