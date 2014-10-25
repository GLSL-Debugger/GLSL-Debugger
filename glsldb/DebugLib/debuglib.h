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

#ifndef DEBUG_LIB_H
#define DEBUG_LIB_H

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void) x;
#endif

#ifdef _WIN32
#include <windows.h>
#if !defined(_MSC_VER) && !defined(WGL_SWAPMULTIPLE_MAX)
typedef struct _WGLSWAP
{
    HDC hdc;
    UINT uiFlags;
} WGLSWAP, *PWGLSWAP, FAR *LPWGLSWAP;
#define WGL_SWAPMULTIPLE_MAX 16
WINGDIAPI DWORD WINAPI wglSwapMultipleBuffers(UINT, CONST WGLSWAP *);
#endif
#endif /* _WIN32 */

#include <stdint.h>

enum DBG_ERROR_CODES {
	DBG_NO_ERROR = 0,
	DBG_ERROR_NO_ACTIVE_SHADER,
	DBG_ERROR_NO_SUCH_DBG_FUNC,
	DBG_ERROR_MEMORY_ALLOCATION_FAILED,
	DBG_ERROR_DBG_SHADER_COMPILE_FAILED,
	DBG_ERROR_DBG_SHADER_LINK_FAILED,
	DBG_ERROR_NO_STORED_SHADER,
	DBG_ERROR_READBACK_INVALID_COMPONENTS,
	DBG_ERROR_READBACK_INVALID_FORMAT,
	DBG_ERROR_READBACK_NOT_ALLOWED,
	DBG_ERROR_OPERATION_NOT_ALLOWED,
	DBG_ERROR_INVALID_OPERATION,
	DBG_ERROR_INVALID_VALUE,
	DBG_ERROR_INVALID_DBG_TARGET,
	DBG_ERROR_VARYING_INACTIVE
};

enum DBG_RETURNS {
	DBG_FUNCTION_CALL = 0,
	/*
	 Returned when a new call is reached and the command loop has been
	 entered.
	 */

	DBG_ERROR_CODE,
	/*
	 Returned by all operations in case of error and additionally by all
	 operations that have no dedicated DBG_RETURN.
	 */

	DBG_RETURN_VALUE,
	/*
	 Returned by a successful call to DBG_CALL_ORIGFUNCTION if the function
	 returns a value
	 */

	DBG_READBACK_RESULT_FRAGMENT_DATA,
	/*
	 Returned by a successful call to DBG_READ_RENDER_BUFFER or
	 DBG_SHADER_STEP
	 */

	DBG_READBACK_RESULT_VERTEX_DATA,
	/*
	 Returned by a successful call to DBG_SHADER_STEP
	 */

	DBG_SHADER_CODE,
	/*
	 Returned by a successful call to DBG_GET_SHADER_CODE
	 */

	DBG_ALLOCATED,
	/*
	 Returned by a successful call to DBG_ALLOC_MEM
	 */

	DBG_DBG_FUNC_RESULT,
	/*
	 Returned by a successful call to a debug function
	 */

	DBG_EXECUTE_IN_PROGRESS
/*
 Returned if a DBG_EXECUTE is in progress
 */
};

enum DBG_OPERATIONS {
	DBG_NONE = 0,
	/*
	 No operation; ignored
	 Parameters:
	 Returns:
	 result : DBG_ERROR_CODE (always DBG_NO_ERROR)
	 */

	DBG_EXECUTE,
	/*
	 Go into "no trace" mode, i.e. do not enter command loop but just call
	 original functions until DBG_STOP_EXECUTION is called or a specified
	 function call is reached
	 Parameters:
	 items[0]: DBG_EXECUTE_RUN - run until DBG_STOP_EXECUTION is set
	 DBG_JUMP_TO_USER_DEFINED - run until user-defined function
	 is reached or DBG_STOP_EXECUTION is set
	 DBG_JUMP_TO_SHADER_SWITCH - run until shader switch
	 reached or DBG_STOP_EXECUTION is set
	 DBG_JUMP_TO_DRAW_CALL - run until drawcall reached or
	 DBG_STOP_EXECUTION is set
	 items[1]: if 1 stop execution on OpenGL error
	 fname   : if items[0] == DBG_JUMP_TO_USER_DEFINED - the name of the function
	 that terminates the run when reached
	 Returns: -
	 */

	DBG_STOP_EXECUTION,
	/*
	 Leave "no trace" mode and go back to normal command loop mode
	 Parameters: -
	 Returns: -
	 */

	DBG_ALLOC_MEM,
	/*
	 Parameters:
	 numItems      : #n of memory blocks to allocate
	 items[0..n-1] : sizes of memory blocks
	 Returns:
	 result        : DBG_ALLOCATED or DBG_ERROR_MEMORY_ALLOCATION_FAILED
	 on error
	 items[0..n-1] : addresses of allocated memory blocks
	 */

	DBG_FREE_MEM,
	/*
	 Parameters:
	 numItems      : #n of memory blocks to free
	 items[0..n-1] : addresses of memory blocks to free
	 Returns:
	 result        : DBG_ERROR_CODE
	 */

	DBG_CALL_FUNCTION,
	/*
	 Call a debug function provided as a shared object with the function
	 calls' parameters.
	 Parameters:
	 depends on called function
	 Returns:
	 depends on called function
	 */

	DBG_CALL_ORIGFUNCTION,
	/*
	 Call the original function.
	 Parameters: -
	 Returns:
	 result : DBG_ERROR_CODE
	 */

	DBG_START_RECORDING,
	/*
	 setup GL state to correctly record and replay a draw call for shader
	 debugging, i.e. save current GL state and init stream recorder.
	 Parameters: -
	 Returns:
	 result : DBG_ERROR_CODE
	 */

	DBG_RECORD_CALL,
	/*
	 Store the current function call including all its parameters and execute
	 the original function afterwards.
	 NOTE: Recording is currently only supported for a subset of the GL
	 calls; see streamrecording.h for details. For all functions that have
	 either have a DBG_STREAM_HINT of DBG_NO_RECORD or do not have a
	 DBG_STREAM_HINT at all calling DBG_RECORD_CALL behaves exactly like
	 DBG_CALL_ORIGFUNCTION.
	 Parameters: -
	 Returns:
	 result : DBG_ERROR_CODE
	 */

	DBG_REPLAY,
	/*
	 Replay the previously recorded GL command stream, i.e. set saved GL
	 state and replay recorded function calls
	 Parameters: -
	 Returns:
	 result : DBG_ERROR_CODE
	 */

	DBG_END_REPLAY,
	/*
	 Cleanup after shader debugging, i.e. restore original output target,
	 restore GL state, replay the recorded stream alast time, and clear the
	 recording.
	 Parameters: -
	 Returns:
	 result : DBG_ERROR_CODE
	 */

	DBG_SET_DBG_TARGET,
	/*
	 Initialize the debug output target
	 Parameters:
	 items[0] : debug target, see DBG_TARGETS below
	 if items[0] == DBG_TARGET_FRAGMENT_SHADER :
	 items[1] : one of DBG_PFT_OPTIONS for alpha test
	 items[2] : one of DBG_PFT_OPTIONS for depth test
	 items[3] : one of DBG_PFT_OPTIONS for stencil test
	 items[4] : one of DBG_PFT_OPTIONS for blending

	 Returns:
	 result: DBG_ERROR_CODE
	 */

	DBG_RESTORE_RENDER_TARGET,
	/*
	 Restore the output target
	 Parameters:
	 items[0] : debug target, see DBG_TARGETS below
	 Returns:
	 result: DBG_ERROR_CODE
	 */

	DBG_READ_RENDER_BUFFER,
	/*
	 Readback the currently active render buffer
	 Parameters:
	 items[0] : number of components to read (1:R, 3:RGB, 4:RGBA)
	 Returns:
	 result   : DBG_READBACK_RESULT_FRAGMENT_DATA or DBG_ERROR_CODE on
	 error
	 items[0] : buffer address
	 items[1] : image width
	 items[2] : image height
	 */

	DBG_CLEAR_RENDER_BUFFER,
	/*
	 Copy or clear the currently active color buffer, depth buffer, and/or
	 stencil buffer
	 Parameters:
	 items[0] : bitwise OR of several values indicating which buffer is
	 to be cleared or copied, allowed values are
	 DBG_CLEAR_RGB, DBG_CLEAR_ALPHA, DBG_CLEAR_DEPTH, and
	 DBG_CLEAR_STENCIL
	 items[1] : clear value red (float!)
	 items[2] : clear value green (float!)
	 items[3] : clear value blue (float!)
	 items[4] : clear value alpha (float!)
	 items[5] : clear value depth (float!)
	 items[6] : clear value stencil
	 Returns:
	 result: DBG_ERROR_CODE
	 */

	DBG_GET_SHADER_CODE,
	/*
	 Return the source code for the currently active GLSL shader
	 Parameters: -
	 Returns:
	 result   : DBG_SHADER_CODE or DBG_ERROR_CODE on error
	 numItems : number of returned shader codes (0 if no shader is
	 active, 3 else)
	 items[0] : pointer to vertex shader src
	 items[1] : length of vertex shader src
	 items[2] : pointer to geometry shader src
	 items[3] : length of geometry shader src
	 items[4] : pointer to fragment shader src
	 items[5] : length of fragment shader src
	 items[6] : pointer to shader resources (TBuiltInResource*)
	 items[7] : number of active uniforms
	 items[8] : pointer to active uniforms (ActiveUniform*), static readonly!
	 */

	DBG_STORE_ACTIVE_SHADER,
	/*
	 Store the currently active GLSL shader including source code, active
	 uniforms, varyings, etc.
	 Parameters: -
	 Returns:
	 result: DBG_ERROR_CODE
	 */

	DBG_RESTORE_ACTIVE_SHADER,
	/*
	 Restore the GLSL shader stored by a previously call to
	 DBG_STORE_ACTIVE_SHADER and free memory used for storing it.
	 Parameters: -
	 Returns:
	 result: DBG_ERROR_CODE
	 */

	DBG_SET_DBG_SHADER,
	/*
	 Set a new, i.e. compile, link, and activate, a new GLSL shader replacing
	 the currently active one.
	 Parameters:
	 items[0] : pointer to vertex shader src
	 items[1] : pointer to geometry shader src
	 items[2] : pointer to fragment shader src
	 items[3] : debug target, see DBG_TARGETS below
	 Returns:
	 result   : DBG_ERROR_CODE
	 */

	DBG_SHADER_STEP,
	/*
	 Does all operations necessary to get the result of a given debug shader
	 back to the caller, i.e. setup the shader and its environment, replay the
	 draw call and readback the result.
	 Parameters:
	 items[0] : pointer to vertex shader src
	 items[1] : pointer to geometry shader src
	 items[2] : pointer to fragment shader src
	 items[3] : debug target, see DBG_TARGETS below
	 if target == DBG_TARGET_FRAGMENT_SHADER:
	 items[4] : number of components to read (1:R, 3:RGB, 4:RGBA)
	 if target == DBG_TARGET_VERTEX_SHADER or DBG_TARGET_GEOMETRY_SHADER:
	 items[4] : primitive mode
	 items[5] : force primitive mode even for geometry shader target
	 items[6] : expected size of debugResult (# floats) per vertex
	 Returns:
	 if target == DBG_TARGET_FRAGMENT_SHADER:
	 result   : DBG_READBACK_RESULT_FRAGMENT_DATA or DBG_ERROR_CODE
	 on error
	 items[0] : buffer address
	 items[1] : image width
	 items[2] : image height
	 if target == DBG_TARGET_VERTEX_SHADER or DBG_TARGET_GEOMETRY_SHADER:
	 result   : DBG_READBACK_RESULT_VERTEX_DATA or DBG_ERROR_CODE on
	 error
	 items[0] : buffer address
	 items[1] : number of vertices
	 items[2] : number of primitives
	 */

	DBG_SAVE_AND_INTERRUPT_QUERIES,
	/*
	 Store the current query state and interrupt active queries.
	 Parameters: -
	 Returns:
	 result   : DBG_ERROR_CODE
	 */

	DBG_RESTART_QUERIES,
	/*
	 Restart interrupted queries.
	 Parameters: -
	 Returns:
	 result   : DBG_ERROR_CODE
	 */

	DBG_DONE
/*
 Quit command loop for the current call and proceed to next call.
 Parameters: -
 Returns:
 result   : DBG_ERROR_CODE
 */
};

enum DBG_TYPES {
	DBG_TYPE_CHAR = 0,
	DBG_TYPE_UNSIGNED_CHAR,
	DBG_TYPE_SHORT_INT,
	DBG_TYPE_UNSIGNED_SHORT_INT,
	DBG_TYPE_INT,
	DBG_TYPE_UNSIGNED_INT,
	DBG_TYPE_LONG_INT,
	DBG_TYPE_UNSIGNED_LONG_INT,
	DBG_TYPE_LONG_LONG_INT,
	DBG_TYPE_UNSIGNED_LONG_LONG_INT,
	DBG_TYPE_FLOAT,
	DBG_TYPE_DOUBLE,
	DBG_TYPE_LONG_DOUBLE,
	DBG_TYPE_POINTER,
	DBG_TYPE_STRUCT,
	DBG_TYPE_BITFIELD,
	DBG_TYPE_ENUM,
	DBG_TYPE_BOOLEAN
};

enum DBG_TARGETS {
	DBG_TARGET_VERTEX_SHADER,
	DBG_TARGET_GEOMETRY_SHADER,
	DBG_TARGET_FRAGMENT_SHADER
};

enum DBG_CLEAR_MODES {
	DBG_CLEAR_NONE = 0,
	DBG_CLEAR_RGB = 1,
	DBG_CLEAR_ALPHA = 2,
	DBG_CLEAR_DEPTH = 4,
	DBG_CLEAR_STENCIL = 8
};

enum DBG_EXECUTE_MODES {
	DBG_EXECUTE_RUN,
	DBG_JUMP_TO_SHADER_SWITCH,
	DBG_JUMP_TO_DRAW_CALL,
	DBG_JUMP_TO_USER_DEFINED
};

enum DBG_PFT_OPTIONS {
	DBG_PFT_KEEP,
	DBG_PFT_FORCE_ENABLED,
	DBG_PFT_FORCE_DISABLED
};

typedef intptr_t ALIGNED_DATA;

#ifdef GLSLDB_OSX
#	define SHM_SIZE (2*1024*1024)
#else
#	define SHM_SIZE (32*1024*1024)
#endif
#define SHM_MAX_FUNCNAME 1024
#define SHM_MAX_THREADS	 16
#ifdef _WIN32
#define SHM_MAX_ITEMS ((SHM_SIZE/SHM_MAX_THREADS - SHM_MAX_FUNCNAME - 5*sizeof(ALIGNED_DATA))/sizeof(ALIGNED_DATA))
#else /* _WIN32 */
#define SHM_MAX_ITEMS ((SHM_SIZE/SHM_MAX_THREADS - SHM_MAX_FUNCNAME - 4*sizeof(ALIGNED_DATA))/sizeof(ALIGNED_DATA))
#endif /* _WIN32 */

typedef struct {
	ALIGNED_DATA threadId;
	ALIGNED_DATA operation;
	ALIGNED_DATA result;
	char fname[SHM_MAX_FUNCNAME];
	ALIGNED_DATA numItems;
	ALIGNED_DATA items[SHM_MAX_ITEMS];
#ifdef _WIN32
	ALIGNED_DATA isRecursing;
#endif /* _WIN32 */
} DbgRec;

typedef struct {
	const char *prefix;
	const char *extname;
	const char *fname;
	int isDebuggableDrawCall;
	int primitiveModeIndex;
	int isShaderSwitch;
	int isFrameEnd;
	int isFramebufferChange;
} GLFunctionList;

#endif
