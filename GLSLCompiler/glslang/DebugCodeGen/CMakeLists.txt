set(SRC_DEBUG_JMP
	DebugJump.cpp
	IntermStack.cpp
)

set(SRC_DEBUG_VAR
	DebugVar.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/.." 
	"${PROJECT_SOURCE_DIR}/glsldb/utils"
)
add_library(DebugJump STATIC ${SRC_DEBUG_JMP})
add_library(DebugVar STATIC ${SRC_DEBUG_VAR})
