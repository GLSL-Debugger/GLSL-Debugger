set(SRC
	ossource.cpp
	main.cpp
)

include_directories(${CURRENT_SOURCE_DIR}/../../ 
	${CURRENT_SOURCE_DIR}/../../Include 
	${CURRENT_SOURCE_DIR}/../../../compiler 
)
add_definitions(-DWIN32 -D_LIB -DNDEBUG)
add_library(Ossource STATIC ${SRC})