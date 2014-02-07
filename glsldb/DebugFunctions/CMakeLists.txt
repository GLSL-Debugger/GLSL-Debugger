set(SRC
	glEnd.c
)

find_package(OpenGL REQUIRED)
if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(DL REQUIRED)
	find_package(X11 REQUIRED)
endif()
if(GLSLDB_WIN)
	set(DL_LIBRARIES "")
	include (GenerateExportHeader)
	set(SRC	${SRC} glEnd.def)
endif()

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib/plugins")

include_directories(${X11_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/glsldb
	${CMAKE_CURRENT_BINARY_DIR}/../DebugLib
)

add_library(glEnd MODULE ${SRC})

target_link_libraries(glEnd
	glenumerants
	utils
	glsldebug
	${DL_LIBRARIES}
	${OPENGL_LIBRARIES}
)

if(GLSLDB_WIN)
	install(TARGETS glEnd LIBRARY DESTINATION ${DISTRIBUTION_DIRECTORY}/plugins)
endif()