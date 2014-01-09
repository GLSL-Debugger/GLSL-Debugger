
include_directories(include)
include_directories(glsl)
include_directories(mapi)
include_directories(mesa)

file(GLOB SRC_MESA mesa/program/*.c mesa/main/*.c)
add_library(mesa_lib ${SRC_MESA})


add_definitions(-DIR_AST_LOCATION -DIR_DEBUG_STATE)

file(GLOB SRC_INTERFACE glslang/Interface/*.cpp)
file(GLOB SRC_VISITORS glslang/Interface/Visitors/*.cpp)
include_directories(
		${PROJECT_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/mesa-glsl
		${PROJECT_SOURCE_DIR}/mesa-glsl/mesa
    	${PROJECT_SOURCE_DIR}/mesa-glsl/glslang/Public
    	${PROJECT_SOURCE_DIR}/mesa-glsl/glslang/Include
    )

add_library(glsl_mesa_interface ${SRC_INTERFACE} ${SRC_VISITORS})

target_link_libraries(glsl_mesa_interface glsl_lib)

add_subdirectory(glsl)
add_subdirectory(tests)
