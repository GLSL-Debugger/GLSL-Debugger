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

#ifndef _PROG_CONTROL_H_
#define _PROG_CONTROL_H_

#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
#include <sys/wait.h>
#endif /* _WIN32 */
#include "errorCodes.h"
#include "functionCall.h"
#include "../GLSLCompiler/glslang/Public/ResourceLimits.h"
#include "attachToProcess.qt.h"

extern "C" {
#include "GL/gl.h"
#include "GL/glext.h"
#include "debuglib.h"
#include "DebugLib/glenumerants.h"
#include "utils/p2pcopy.h"
}

class ProgramControl {

public:
	ProgramControl(const char *pname);
	~ProgramControl();

	/* remote program control */
	pcErrorCode runProgram(char **debuggedProgramArgs, char *workDir = NULL);
#ifdef _WIN32
	pcErrorCode attachToProgram(const DWORD pid);
#else /* _WIN32 */
	pcErrorCode attachToProgram(const pid_t pid);
#endif /* _WIN32 */
	pcErrorCode killProgram(int hard = 0);
	pcErrorCode detachFromProgram(void);
	bool childAlive(void);
	pcErrorCode checkChildStatus(void);
	FunctionCall* getCurrentCall(void);

	pcErrorCode execute(bool stopOnGLError);
	pcErrorCode executeToShaderSwitch(bool stopOnGLError);
	pcErrorCode executeToDrawCall(bool stopOnGLError);
	pcErrorCode executeToUserDefined(const char *fname, bool stopOnGLError);
	pcErrorCode checkExecuteState(int *state);
	pcErrorCode executeContinueOnError(void);
	pcErrorCode stop(void);

	pcErrorCode callOrigFunc(const FunctionCall *fCall = 0);
	pcErrorCode callDone(void);
	pcErrorCode overwriteFuncArguments(const FunctionCall *fCall);

	pcErrorCode restoreRenderTarget(int target);
	pcErrorCode setDbgTarget(int target, int alphaTestOption,
			int depthTestOption, int stencilTestOption, int blendingOption);

	pcErrorCode saveAndInterruptQueries(void);
	pcErrorCode restartQueries(void);

	pcErrorCode initRecording(void);
	pcErrorCode recordCall(void);
	pcErrorCode replay(int target);
	pcErrorCode endReplay(void);

	pcErrorCode getShaderCode(char *shaders[3], TBuiltInResource *resource,
			char **serializedUniforms, int *numUniforms);

	pcErrorCode saveActiveShader(void);
	pcErrorCode restoreActiveShader(void);

	pcErrorCode shaderStepFragment(char *shaders[3], int numComponents,
			int format, int *width, int *heigh, void **image);
	pcErrorCode shaderStepVertex(char *shaders[3], int target,
			int primitiveMode, int forcePointPrimitiveMode,
			int numFloatsPerVertex, int *numPrimitives, int *numVertices,
			float **vertexData);

	/* obsolete? */
	pcErrorCode setDbgShaderCode(char *shaders[3], int target);

	pcErrorCode initializeRenderBuffer(bool copyRGB, bool copyAlpha,
			bool copyDepth, bool copyStencil, float red, float green,
			float blue, float alpha, float depth, int stencil);
	pcErrorCode readBackActiveRenderBuffer(int numComponents, int *width,
			int *heigh, float **image);

	pcErrorCode insertGlEnd(void);

private:
	unsigned int getArgumentSize(int type);

	/* process environment communication */
	void buildEnvVars(const char *pname);
	void setDebugEnvVars(void);

	/* remote program control */
#ifndef _WIN32
	pid_t _debuggeePID;
#else /* _WIN32 */
	DWORD _debuggeePID;
#endif /* _WIN32 */
	pcErrorCode dbgCommandExecute(void);
	pcErrorCode dbgCommandExecuteToDrawCall(void);
	pcErrorCode dbgCommandExecuteToShaderSwitch(void);
	pcErrorCode dbgCommandExecuteToUserDefined(const char *fname);
	pcErrorCode dbgCommandExecute(bool stopOnGLError);
	pcErrorCode dbgCommandExecuteToDrawCall(bool stopOnGLError);
	pcErrorCode dbgCommandExecuteToShaderSwitch(bool stopOnGLError);
	pcErrorCode dbgCommandExecuteToUserDefined(const char *fname,
			bool stopOnGLError);
	pcErrorCode dbgCommandStopExecution(void);
	pcErrorCode dbgCommandCallOrig(void);
	pcErrorCode dbgCommandCallOrig(const FunctionCall *fCall);
	pcErrorCode dbgCommandCallDBGFunction(const char* = 0);
	pcErrorCode dbgCommandAllocMem(unsigned int numBlocks, unsigned int *sizes,
			void **addresses);
	pcErrorCode dbgCommandFreeMem(unsigned int numBlocks, void **addresses);
	pcErrorCode dbgCommandStartRecording(void);
	pcErrorCode dbgCommandRecord(void);
	pcErrorCode dbgCommandReplay(int target);
	pcErrorCode dbgCommandEndReplay(void);
	pcErrorCode dbgCommandSetDbgTarget(int target, int alphaTestOption,
			int depthTestOption, int stencilTestOption, int blendingOption);
	pcErrorCode dbgCommandRestoreRenderTarget(int target);
	pcErrorCode dbgCommandCopyToRenderBuffer(void);
	pcErrorCode dbgCommandClearRenderBuffer(int mode, float r, float g, float b,
			float a, float f, int s);
	pcErrorCode dbgCommandSaveAndInterruptQueries(void);
	pcErrorCode dbgCommandRestartQueries(void);
	pcErrorCode dbgCommandShaderStepFragment(void *shaders[3],
			int numComponents, int format, int *width, int *height,
			void **image);
	pcErrorCode dbgCommandShaderStepVertex(void *shaders[3], int target,
			int primitiveMode, int forcePointPrimitiveMode,
			int numFloatsPerVertex, int *numPrimitives, int *numVertices,
			float **vertexData);
	pcErrorCode dbgCommandReadRenderBuffer(int numComponents, int *width,
			int *height, float **image);
	pcErrorCode dbgCommandDone(void);
	void* copyArgumentFromProcess(void *addr, int type);
	void copyArgumentToProcess(void *dst, void *src, int type);
	char* printArgument(void *addr, int type);
	void printCall(void);
	void printResult(void);

	/* dbg command execution and error checking */
	pcErrorCode executeDbgCommand(void);
	pcErrorCode checkError(void);

	/* Shared memory handling */
	void initShmem(void);
	void clearShmem(void);
	void freeShmem(void);
#ifndef _WIN32
	DbgRec* getThreadRecord(pid_t pid);
#else /* _WIN32 */
	DbgRec* getThreadRecord(DWORD pid);
#endif /* _WIN32 */

	int shmid;
	DbgRec *_fcalls;
	std::string _path_dbglib;
	std::string _path_dbgfuncs;
	std::string _path_libdlsym;
	std::string _path_log;

#ifdef _WIN32
	void createEvents(const DWORD processId);
	void closeEvents(void);

	/* Signal debugee. */
	HANDLE _hEvtDebuggee;

	/* Wait for debugee signaling me. */
	HANDLE _hEvtDebugger;

	/* Process handle of the debugged program. */
	HANDLE _hDebuggedProgram;

	/** Handles for process we attached to, but did not create ourself. */
	ATTACHMENT_INFORMATION _ai;

	HANDLE _hShMem;

#endif /* _WIN32 */
};

#endif
