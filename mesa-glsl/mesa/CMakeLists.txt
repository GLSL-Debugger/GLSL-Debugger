
include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mapi"
	"${CMAKE_CURRENT_SOURCE_DIR}/glsl"
	"${CMAKE_CURRENT_SOURCE_DIR}/util"
	"${PROJECT_SOURCE_DIR}/glsldb"
)

if (MSVC)
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/c99")
endif()

file(GLOB SRC_MESA mesa/program/*.c mesa/main/*.c)
file(GLOB SRC_MESA_UTIL util/*c)
add_library(mesa_util ${SRC_MESA_UTIL})
add_library(mesa_lib ${SRC_MESA})
target_link_libraries(mesa_lib mesa_util)

add_subdirectory(glsl)
