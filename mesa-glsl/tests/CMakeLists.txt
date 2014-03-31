
find_package(CppUnit REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(TESTS_SRC
	runner.cpp
	misc.cpp
	ShaderInput.cpp
	call_visitor.cpp
)

add_executable(glsl_tests ${TESTS_SRC})
target_link_libraries(glsl_tests glsl_mesa_interface utils ${CPPUNIT_LIBRARY})

add_test(NAME "TestMesaGLSL" COMMAND "${BINARY_DIR}/glsl_tests" "${CMAKE_CURRENT_SOURCE_DIR}/")

# Not sure why I did this.
execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/generate_shader_checker.sh" "${BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_FILE "${BINARY_DIR}/copy_shaders.sh"
)
