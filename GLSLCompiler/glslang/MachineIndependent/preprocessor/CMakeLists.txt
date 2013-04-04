set(SRC
	atom.c
	cpp.c
	cppstruct.c
	memory.c
	scanner.c	
	symbols.c 
	tokens.c 
)

add_library(Preprocessor STATIC ${SRC})