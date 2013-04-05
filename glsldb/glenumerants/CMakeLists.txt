# FIXME
# add windows support
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

add_library(glenumerants STATIC ${SRC})