set(SRC	
	glEnd.c
)

find_package(DL REQUIRED)
find_package(OpenGL REQUIRED)

set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/plugins")
add_library(glEnd MODULE ${SRC})
target_link_libraries(glEnd glenumerants utils ${DL_LIBRARIES} ${OPENGL_LIBRARIES})