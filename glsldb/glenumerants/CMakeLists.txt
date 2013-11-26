# FIXME
# add windows support

if(GLSLDB_LINUX OR GLSLDB_OSX)
    find_package(X11 REQUIRED)
endif()

set(SRC glenumerants.c)

execute_process(COMMAND perl gen_enumerants.pl ../GL/gl.h ../GL/glext.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    INPUT_FILE gen_enumerants.pl
    OUTPUT_FILE enumerants.h
)
execute_process(COMMAND perl gen_enumerants.pl -m glx ../GL/glx.h ../GL/glxext.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    INPUT_FILE gen_enumerants.pl
    OUTPUT_FILE glxenumerants.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../
    ${X11_INCLUDE_DIR}
)

add_library(glenumerants STATIC ${SRC})
