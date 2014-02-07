add_definitions(-DUNICODE -D_UNICODE)
set(SRC
	disasm-lib/cpu.c
	disasm-lib/disasm.c
	disasm-lib/disasm_x86.c
	disasm-lib/misc.c
	mhook-lib/mhook.cpp
)
add_library(mhook STATIC ${SRC})
