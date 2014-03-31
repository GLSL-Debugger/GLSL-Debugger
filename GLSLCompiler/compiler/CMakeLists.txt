set(SRC InitializeDll.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../glslang" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../glslang/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../glslang/MachineIndependent"
	${OSDEPENDENT_INCLUDES}
)
add_definitions(-D__i386__ -D_ALT_NS=1)
add_library(InitializeDll STATIC ${SRC})
