set(SRC
	hash.c 
	dlutils.c 
	p2pcopy.c 
	dbgprint.c 
	pfm.c 
	sync.c 
)

add_library(utils STATIC ${SRC})