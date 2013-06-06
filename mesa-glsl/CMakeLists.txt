
include_directories(include)
include_directories(glsl)
include_directories(mapi)
include_directories(mesa)


file(GLOB SRC_MESA mesa/program/*.c mesa/main/*.c)
add_library(glsl_mesa ${SRC_MESA})

add_subdirectory(glsl)
