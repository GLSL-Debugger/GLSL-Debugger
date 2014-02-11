

add_executable(glsl_tests runner.cpp misc.cpp)
target_link_libraries(glsl_tests glsl_mesa_interface utils)