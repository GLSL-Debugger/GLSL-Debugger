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

#include <stdlib.h>
#include <stdio.h>

#include "dbgprint.h"
#include "debuglib.h"
#include "debuglibInternal.h"
#include "glstate.h"
#include "readback.h"
#ifdef _WIN32
#include "trampolines.h"
#endif /* _WIN32 */

int saveGLState(void)
{
	int error;
	
	DMARK
	/* save original gl state */
	ORIG_GL(glPushAttrib)(GL_ALL_ATTRIB_BITS);
	error = glError();
	if (error) {
		return error;
	}
	
	/*glPushClientAttrib*/
	
	return DBG_NO_ERROR;
}

int setSavedGLState(int target)
{ 
	int error;
	
	DMARK
	/* restore original gl state */
	ORIG_GL(glPopAttrib)();
	error = glError();
	if (error) {
		return error;
	}
	
	/* save original gl state */
	error = saveGLState();
	if (error) {
		return error;
	}
	
/* FIXME CHECK AGAIN!!!! 	*/
#if 0
	/* disable everything that could interfere when writting the debug result to
	 * the debug buffer and setup draw and read buffer.
	 */
	error = setDbgRenderState(target);
	if (error) {
		return error;
	}
#endif	
	return DBG_NO_ERROR;
}

int restoreGLState(void)
{
	DMARK
	/* restore original gl state */
	ORIG_GL(glPopAttrib)();
	return glError();
}

