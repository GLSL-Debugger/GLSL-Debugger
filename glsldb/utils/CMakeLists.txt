set(SRC
	hash.c 
	dlutils.c 
	p2pcopy.c 
	dbgprint.c 
	pfm.c 
	sync.c 
	notify.c
)

if(GLSLDB_WIN)
	set(SRC ${SRC} getopt.c)
endif()


add_library(utils STATIC ${SRC})
