# FIXME
# add windows support

if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(X11 REQUIRED)
endif()

set(SRC
	glenumerants.c
	glxenumerants.c
)

execute_process(COMMAND perl genEnumerants.pl ../GL/gl.h ../GL/glext.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	INPUT_FILE genEnumerants.pl
	OUTPUT_FILE enumerants.h
)
execute_process(COMMAND perl genGLXEnumerants.pl ../GL/glx.h ../GL/glxext.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	INPUT_FILE genGLXEnumerants.pl
	OUTPUT_FILE glxenumerants.h
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../
	${X11_INCLUDE_DIR}
)
add_library(glenumerants STATIC ${SRC})