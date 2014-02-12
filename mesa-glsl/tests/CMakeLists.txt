
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TESTS_SRC
	runner.cpp
	misc.cpp
	ShaderInput.cpp
)

add_executable(glsl_tests ${TESTS_SRC})
target_link_libraries(glsl_tests glsl_mesa_interface utils cppunit)