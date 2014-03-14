set(SRC
	CodeGen.cpp
	Link.cpp
	CodeInsertion.cpp
	CodeTools.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../"
	"${PROJECT_SOURCE_DIR}/glsldb/utils"
)
add_library(CodeGen STATIC ${SRC})
