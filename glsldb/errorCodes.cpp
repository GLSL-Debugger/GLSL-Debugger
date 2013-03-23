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

#include "errorCodes.h"

bool isErrorCritical(pcErrorCode error)
{
    switch (error) {
        case PCE_NONE:
		/* gl errors */
        case PCE_GL_INVALID_ENUM:
        case PCE_GL_INVALID_VALUE:
        case PCE_GL_INVALID_OPERATION:
        case PCE_GL_STACK_OVERFLOW:
        case PCE_GL_STACK_UNDERFLOW:
        case PCE_GL_OUT_OF_MEMORY:
        case PCE_GL_TABLE_TOO_LARGE:
		case PCE_GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		/* other non-critical errors */
		case PCE_DBG_READBACK_NOT_ALLOWED:
            return false;
        default:
            return true;
    }
}

bool isOpenGLError(pcErrorCode error)
{
	switch (error) {
		/* gl errors */
        case PCE_GL_INVALID_ENUM:
        case PCE_GL_INVALID_VALUE:
        case PCE_GL_INVALID_OPERATION:
        case PCE_GL_STACK_OVERFLOW:
        case PCE_GL_STACK_UNDERFLOW:
        case PCE_GL_OUT_OF_MEMORY:
        case PCE_GL_TABLE_TOO_LARGE:
		case PCE_GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
			return true;
		default:
			return false;
	}
}

const char* getErrorDescription(pcErrorCode error)
{
    switch (error) {
		/* general debugger errors */
		case PCE_NONE:
			return "";
        case PCE_FORK:
            return "Error forking child process";
        case PCE_EXEC:
            return "Error executing child process";
        case PCE_EXIT:
            return "Child process exited";
		case PCE_UNKNOWN_ERROR:
            return "Internal debugger error";
		case PCE_MEMORY_ALLOCATION_FAILED:
			return "Memory allocation failed";
		/* debuglib errors */
		case PCE_DBG_NO_ACTIVE_SHADER:
		case PCE_DBG_NO_SUCH_DBG_FUNC:
		case PCE_DBG_MEMORY_ALLOCATION_FAILED:
		case PCE_DBG_DBG_SHADER_COMPILE_FAILED:
		case PCE_DBG_DBG_SHADER_LINK_FAILED:
		case PCE_DBG_NO_STORED_SHADER:
		case PCE_DBG_READBACK_INVALID_COMPONENTS:
		case PCE_DBG_READBACK_INVALID_FORMAT:
		case PCE_DBG_OPERATION_NOT_ALLOWED:
		case PCE_DBG_INVALID_OPERATION:
		case PCE_DBG_INVALID_VALUE:
            return "Internal debugger error";
		case PCE_DBG_READBACK_NOT_ALLOWED:
			return "Readback not allowed here";
		/* gl errors */
        case PCE_GL_INVALID_ENUM:
            return "OpenGL error GL_INVALID_ENUM detected";
        case PCE_GL_INVALID_VALUE:
            return "OpenGL error GL_INVALID_VALUE detected";
        case PCE_GL_INVALID_OPERATION:
            return "OpenGL error GL_INVALID_OPERATION detected";
        case PCE_GL_STACK_OVERFLOW:
            return "OpenGL error GL_STACK_OVERFLOW detected";
        case PCE_GL_STACK_UNDERFLOW:
            return "OpenGL error GL_STACK_UNDERFLOW detected";
        case PCE_GL_OUT_OF_MEMORY:
            return "OpenGL error GL_OUT_OF_MEMORY detected";
        case PCE_GL_TABLE_TOO_LARGE:
            return "OpenGL error GL_TABLE_TOO_LARGE detected";
		case PCE_GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
			return "PCE_GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
        default:
            return "UNKNOWN ERROR!";
    }
}

const char* getErrorInfo(pcErrorCode error)
{
    switch (error) {
		case PCE_NONE:
			return "";
        case PCE_FORK:
            return "FORK_FAILED";
        case PCE_EXEC:
            return "EXEC_FAILED";
        case PCE_EXIT:
            return "CHILD_EXIT";
        case PCE_UNKNOWN_ERROR:
            return "PROGRAM_ERROR";
		/* debuglib errors */
		case PCE_MEMORY_ALLOCATION_FAILED:
		case PCE_DBG_NO_ACTIVE_SHADER:
		case PCE_DBG_NO_SUCH_DBG_FUNC:
		case PCE_DBG_MEMORY_ALLOCATION_FAILED:
		case PCE_DBG_DBG_SHADER_COMPILE_FAILED:
		case PCE_DBG_DBG_SHADER_LINK_FAILED:
		case PCE_DBG_NO_STORED_SHADER:
		case PCE_DBG_READBACK_INVALID_COMPONENTS:
		case PCE_DBG_READBACK_INVALID_FORMAT:
		case PCE_DBG_OPERATION_NOT_ALLOWED:
		case PCE_DBG_INVALID_OPERATION:
		case PCE_DBG_INVALID_VALUE:
            return "INTERNAL_ERROR";
		case PCE_DBG_READBACK_NOT_ALLOWED:
			return "INVALID_OPERATION";
		/* gl errors */
        case PCE_GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case PCE_GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case PCE_GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case PCE_GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case PCE_GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case PCE_GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case PCE_GL_TABLE_TOO_LARGE:
            return "GL_TABLE_TOO_LARGE";
		case PCE_GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
			return "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
        default:
            return "UNKNOW";
    }
}

