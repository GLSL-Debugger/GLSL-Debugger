
add_definitions(-DAST_DEBUG_STATE -DHAVE_PTHREAD)
add_subdirectory(mesa)

include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/glsldb"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/interface"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa/mesa"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/mesa/mapi"
)

if (MSVC)
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/c99")
endif()

file(GLOB SRC_INTERFACE interface/*.cpp)
file(GLOB SRC_VISITORS interface/visitors/*.cpp)

add_library(glsl_mesa_interface ${SRC_INTERFACE} ${SRC_VISITORS})
target_link_libraries(glsl_mesa_interface glsl_lib)

if(TESTS)
    add_subdirectory(tests)
endif()
