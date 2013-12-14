set(SRC	
	glEnd.c
)

find_package(DL REQUIRED)
find_package(OpenGL REQUIRED)
if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(X11 REQUIRED)
endif()

set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/plugins")

include_directories(${X11_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/glsldb
)

add_library(glEnd MODULE ${SRC})

target_link_libraries(glEnd 
	glenumerants 
	utils
	glsldebug
	${DL_LIBRARIES} 
	${OPENGL_LIBRARIES}
)