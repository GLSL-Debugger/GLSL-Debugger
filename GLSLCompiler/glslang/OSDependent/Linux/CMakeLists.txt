set(SRC
	ossource.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../ 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler 
)
add_definitions(-D__i386__ -D_ALT_NS=1)
add_library(Ossource STATIC ${SRC})