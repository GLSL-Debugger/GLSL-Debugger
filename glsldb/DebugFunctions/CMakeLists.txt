set(SRC
	glEnd.c
)

set(GLEND_OS_DEPENDENT_LIBS "")
set(GLEND_OS_DEPENDENT_INCLUDES "")

find_package(OpenGL REQUIRED)
if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(DL REQUIRED)
	find_package(X11 REQUIRED)
	set(GLEND_OS_DEPENDENT_LIBS ${DL_LIBRARIES})
	set(GLEND_OS_DEPENDENT_INCLUDES ${X11_INCLUDE_DIR})
endif()
if(GLSLDB_WIN)
	set(DL_LIBRARIES "")
	include (GenerateExportHeader)
	set(SRC	${SRC} glEnd.def)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}/plugins")

include_directories(
	"${PROJECT_SOURCE_DIR}/glsldb"
	"${CMAKE_BINARY_DIR}/glsldb/DebugLib"
	${GLEND_OS_DEPENDENT_INCLUDES}
)

add_library(glEnd MODULE ${SRC})

target_link_libraries(glEnd
	glenumerants
	utils
	glsldebug
	${OPENGL_LIBRARIES}
	${GLEND_OS_DEPENDENT_LIBS}
)

if(GLSLDB_WIN)
	install(TARGETS glEnd LIBRARY DESTINATION "${DISTRIBUTION_DIRECTORY}/plugins")
endif()
