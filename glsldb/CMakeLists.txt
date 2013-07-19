add_subdirectory(DebugLib)
add_subdirectory(glenumerants)
add_subdirectory(DebugFunctions)
add_subdirectory(utils)

set(QT_MIN_VERSION 4.4.0)
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if(GLSLDB_LINUX OR GLSLDB_OSX)
	find_package(X11 REQUIRED)
	find_package(Threads REQUIRED)
endif()

include(${QT_USE_FILE})

file(GLOB SRC_CPP *.cpp)
file(GLOB SRC_C *.c)

file(GLOB MOC_HEADERS *.qt.h)
file(GLOB RES_FILES *.qrc)
file(GLOB UI_FILES ui/*.ui)
qt4_wrap_cpp(MOC_OUTFILES ${MOC_HEADERS})
qt4_wrap_ui(UI_OUTFILES ${UI_FILES})
qt4_add_resources(RES_OUTFILES ${RES_FILES})

set(SRC	
	${SRC_C}
	${SRC_CPP}
	${MOC_OUTFILES}
	${UI_OUTFILES}
	${RES_OUTFILES}
)

include_directories(${OPENGL_INCLUDE_DIR} 
	${X11_INCLUDE_DIR}
	${QT_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/GLSLCompiler/glslang/Public
	${CMAKE_CURRENT_SOURCE_DIR}/utils
	${CMAKE_CURRENT_SOURCE_DIR}/DebugLib
	${GLEW_INCLUDE_DIR}
)

if(GLSLDB_LINUX)
	#add_definitions(-DUNIX)
endif()

add_executable(glsldb ${SRC})
target_link_libraries(glsldb 
	${QT_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT} 
	glslang 
	glenumerants
	functionList
	utils
	${OPENGL_LIBRARIES}
	${X11_LIBRARIES}
	${GLEW_LIBRARIES}
)
