/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifdef _WIN32
/* Make gl.h believe we are Windows. This is required for correct linkage. */
#define _GDI32_
#endif /* _WIN32 */

#include <stdio.h>
#include <stdlib.h>

#define DBGLIB_EXTERNAL

#include "../debuglib.h"
#include "../DebugLib/debuglibInternal.h"
#include "../utils/dbgprint.h"

#ifdef _WIN32
#include "../DebugLib/trampolines.h"
#endif /* _WIN32 */

/*
 * Note (Windows): 'provides' and 'glEnd' are exported using the module 
 * definition file and not by __declspec. We cannot achieve a consistent 
 * storage class with __declspec, and as the symbols are retrieved by 
 * GetProcAddress anyway, allowing only explicit binding does not hurt.
 */

const char provides[] = "glEnd";

void APIENTRY glEnd(void)
{
#if 0
	/* HAZARD maximally dangerous. this relies upon the fact that the debuglib is already loaded
	   (sure, this here lib is loaded by it) to grab the correct address of the opengl32.dll
	   glEnd. We do this because the contents of OrigglEnd are, as a matter of fact, fucked
	   in the head and we end up in a totally wrong memory segment. This might have to do
	   something with improper relocation of all the library goo, but we do not know. yet. */
	HANDLE lib = LoadLibraryA("debuglib.dll");
	dbgPrint(DBGLVL_DEBUG, "using special glEnd: 0x%x\n", OrigglEnd);
	dbgPrint(DBGLVL_DEBUG, "origglend = 0x%x\n", *((GetProcAddress(lib, "OrigglEnd"))));
	(*((GetProcAddress(lib, "OrigglEnd"))))();
	(*((GetProcAddress(lib, "OrigglGetError"))))();
	FreeLibrary(lib);
#endif
	ORIG_GL(glEnd)();
	setErrorCode(ORIG_GL(glGetError)());
}

#ifdef _WIN32
#undef _GDI32_
#endif /* _WIN32 */
