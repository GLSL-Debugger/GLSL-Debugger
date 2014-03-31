
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mapi"
	"${CMAKE_CURRENT_SOURCE_DIR}/glsl"
)

file(GLOB SRC_MESA mesa/program/*.c mesa/main/*.c)
add_library(mesa_lib ${SRC_MESA})

add_subdirectory(glsl)