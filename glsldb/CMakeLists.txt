add_subdirectory(DebugLib)
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

set(GLSLDB_OS_DEPENDENT_INCLUDES "")
set(GLSLDB_OS_DEPENDENT_LIBS "")

if(GLSLDB_LINUX)
	#add_definitions(-DUNIX)
	include_directories()
	list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/progControlWin.cpp")
	set(GLSLDB_OS_DEPENDENT_INCLUDES ${X11_INCLUDE_DIR})
	set(GLSLDB_OS_DEPENDENT_LIBS ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()
if(GLSLDB_WIN)
	set(GLSLDB_OS_DEPENDENT_INCLUDES
		"${CMAKE_CURRENT_SOURCE_DIR}/../GLSLCompiler/glslang/OSDependent/Windows"
	)
endif()

if(USE_MESA)
    set(GLSL_INCLUDES "${PROJECT_SOURCE_DIR}/mesa-glsl/include")
    set(GLSL_LIBRARIES glsl_mesa_interface)
else()
    set(GLSL_INCLUDES
    	"${PROJECT_SOURCE_DIR}/GLSLCompiler/glslang/Public"
    	"${PROJECT_SOURCE_DIR}/GLSLCompiler/glslang/Include"
    )
    set(GLSL_LIBRARIES glslang)
endif()

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/utils"
	"${CMAKE_CURRENT_SOURCE_DIR}/DebugLib"
	${OPENGL_INCLUDE_DIR}
	${QT_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLSLDB_OS_DEPENDENT_INCLUDES}
	${GLSL_INCLUDES}
)

add_executable(glsldb ${SRC})
target_link_libraries(glsldb
	glenumerants
	functionList
	utils
	${QT_LIBRARIES}
	${OPENGL_LIBRARIES}
	${X11_LIBRARIES}
	${GLEW_LIBRARIES}
	${GLSLDB_OS_DEPENDENT_LIBS}
	${GLSL_LIBRARIES}
)

if(GLSLDB_WIN)
	install(TARGETS glsldb RUNTIME DESTINATION ${DISTRIBUTION_DIRECTORY})
endif()
