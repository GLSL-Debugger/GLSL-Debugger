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

#include <string.h>
#include <stdio.h>
#ifndef _WIN32
#include <pthread.h>
#else /* !_WIN32 */
#include <windows.h>
#include "../GL/WinGDI.h"
#define GL_GLEXT_PROTOTYPES 1
#define WGL_WGLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include "../GL/glext.h"
#include "../GL/wglext.h"
#include "detours.h"
#include "trampolines.h"
#endif /* !_WIN32 */

#include "debuglibInternal.h"
#include "streamRecording.h"
#include "../utils/dbgprint.h"

extern Globals G;

#include "preExecution.h"
#include "postExecution.h"
#ifdef _WIN32
#include "trampolines.inc"
#endif /* _WIN32 */
#if defined(GLSLDB_LINUX) || defined(GLSLDB_OSX)
#include "../GL/glxext.h"
#endif
#include "functionHooks.inc"
#include "getProcAddressHook.inc"
