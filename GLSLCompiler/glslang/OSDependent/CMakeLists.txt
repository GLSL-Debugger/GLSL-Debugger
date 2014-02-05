if(GLSLDB_LINUX OR GLSLDB_OSX)
	add_subdirectory(Linux)
endif()

if(GLSLDB_WIN)
	add_subdirectory(Windows)
endif()