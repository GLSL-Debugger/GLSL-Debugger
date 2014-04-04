set(SRC
	ossource.cpp
	main.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler"
	${COMPILER_OS_DEPENDENT_INCLUDES}
)
add_definitions(-DWIN32 -D_LIB -DNDEBUG)
add_library(Ossource STATIC ${SRC})
