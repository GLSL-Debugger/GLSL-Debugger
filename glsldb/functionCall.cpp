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
#include <windows.h>
#include "asprintf.h"
#endif /* _WIN32 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "GL/glext.h"
#include "../utils/dbgprint.h"
#include "errno.h"
#include "debuglib.h"
#include "functionCall.h"
#include "FunctionsMap.h"

extern "C" {
#include "DebugLib/glenumerants.h"
}

//extern "C" GLFunctionList glFunctions[];

FunctionCall::FunctionCall()
{
	m_pName = NULL;
	m_iNumArgs = 0;
	m_pArguments = NULL;
}

FunctionCall::FunctionCall(const FunctionCall *copyOf)
{
	int i;
	m_pName = strdup(copyOf->getName());
	m_iNumArgs = 0;
	m_pArguments = NULL;
	for (i = 0; i < copyOf->getNumArguments(); i++) {
		const Argument *arg = copyOf->getArgument(i);
		addArgument(arg->iType, copyArgument(arg->iType, arg->pData),
				arg->pAddress);
	}
}

FunctionCall::~FunctionCall()
{
	int i;
	for (i = 0; i < m_iNumArgs; i++) {
		free(m_pArguments[i].pData);
	}
	free(m_pArguments);
	free(m_pName);
}

const char* FunctionCall::getName(void) const
{
	return m_pName;
}

void FunctionCall::setName(const char *i_pName)
{
	if (m_pName) {
		free(m_pName);
	}
	m_pName = strdup(i_pName);
}

const char* FunctionCall::getExtension(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	return f ? f->extname : 0;
}

int FunctionCall::getNumArguments(void) const
{
	return m_iNumArgs;
}

const FunctionCall::Argument* FunctionCall::getArgument(int i_iIdx) const
{
	if (0 <= i_iIdx && i_iIdx < m_iNumArgs) {
		return &(m_pArguments[i_iIdx]);
	}
	return NULL;
}

void FunctionCall::addArgument(int i_iType, void *i_pData, void *i_pAddress)
{
	void *tmpAlloc;

	m_iNumArgs++;
	tmpAlloc = realloc(m_pArguments, m_iNumArgs * sizeof(Argument));
	if (!tmpAlloc) {
		dbgPrint(DBGLVL_ERROR,
				"Failed to allocate memory for function argument: %s\n", strerror(errno));
	}
	m_pArguments = (Argument*) tmpAlloc;
	m_pArguments[m_iNumArgs - 1].iType = i_iType;
	m_pArguments[m_iNumArgs - 1].pData = i_pData;
	m_pArguments[m_iNumArgs - 1].pAddress = i_pAddress;
}

void FunctionCall::editArgument(int i_iIdx, void *i_pData)
{
	if (0 <= i_iIdx && i_iIdx < m_iNumArgs) {
		free(m_pArguments[i_iIdx].pData);
		m_pArguments[i_iIdx].pData = copyArgument(m_pArguments[i_iIdx].iType,
				i_pData);
	}
}

void* FunctionCall::copyArgument(int type, void *addr)
{
	void *r;

	switch (type) {
	case DBG_TYPE_CHAR:
		r = malloc(sizeof(char));
		memcpy(r, addr, sizeof(char));
		break;
	case DBG_TYPE_UNSIGNED_CHAR:
		r = malloc(sizeof(unsigned char));
		memcpy(r, addr, sizeof(unsigned char));
		break;
	case DBG_TYPE_SHORT_INT:
		r = malloc(sizeof(short));
		memcpy(r, addr, sizeof(short));
		break;
	case DBG_TYPE_UNSIGNED_SHORT_INT:
		r = malloc(sizeof(unsigned short));
		memcpy(r, addr, sizeof(unsigned short));
		break;
	case DBG_TYPE_INT:
		r = malloc(sizeof(int));
		memcpy(r, addr, sizeof(int));
		break;
	case DBG_TYPE_UNSIGNED_INT:
		r = malloc(sizeof(unsigned int));
		memcpy(r, addr, sizeof(unsigned int));
		break;
	case DBG_TYPE_LONG_INT:
		r = malloc(sizeof(long));
		memcpy(r, addr, sizeof(long));
		break;
	case DBG_TYPE_UNSIGNED_LONG_INT:
		r = malloc(sizeof(unsigned long));
		memcpy(r, addr, sizeof(unsigned long));
		break;
	case DBG_TYPE_LONG_LONG_INT:
		r = malloc(sizeof(long long));
		memcpy(r, addr, sizeof(long long));
		break;
	case DBG_TYPE_UNSIGNED_LONG_LONG_INT:
		r = malloc(sizeof(unsigned long long));
		memcpy(r, addr, sizeof(unsigned long long));
		break;
	case DBG_TYPE_FLOAT:
		r = malloc(sizeof(float));
		memcpy(r, addr, sizeof(float));
		break;
	case DBG_TYPE_DOUBLE:
		r = malloc(sizeof(double));
		memcpy(r, addr, sizeof(double));
		break;
	case DBG_TYPE_POINTER:
		r = malloc(sizeof(void*));
		memcpy(r, addr, sizeof(void*));
		break;
	case DBG_TYPE_BOOLEAN:
		r = malloc(sizeof(GLboolean));
		memcpy(r, addr, sizeof(GLboolean));
		break;
	case DBG_TYPE_BITFIELD:
		r = malloc(sizeof(GLbitfield));
		memcpy(r, addr, sizeof(GLbitfield));
		break;
	case DBG_TYPE_ENUM:
		r = malloc(sizeof(GLbitfield));
		memcpy(r, addr, sizeof(GLbitfield));
		break;
	case DBG_TYPE_STRUCT:
		r = NULL; /* FIXME */
		break;
	default:
		r = NULL;
		break;
	}
	return r;
}

bool FunctionCall::operator==(const FunctionCall &right)
{
	int i;

	if (strcmp(m_pName, right.getName()) != 0) {
		return false;
	}
	if (m_iNumArgs != right.getNumArguments()) {
		return false;
	}

	for (i = 0; i < m_iNumArgs; i++) {
		const FunctionCall::Argument *rArg = right.getArgument(i);
		if (m_pArguments[i].iType != rArg->iType) {
			return false;
		}
		switch (m_pArguments[i].iType) {
		case DBG_TYPE_CHAR:
			if (*(char*) m_pArguments[i].pData != *(char*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_UNSIGNED_CHAR:
			if (*(unsigned char*) m_pArguments[i].pData
					!= *(unsigned char*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_SHORT_INT:
			if (*(short*) m_pArguments[i].pData != *(short*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_UNSIGNED_SHORT_INT:
			if (*(unsigned short*) m_pArguments[i].pData
					!= *(unsigned short*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_INT:
			if (*(int*) m_pArguments[i].pData != *(int*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_UNSIGNED_INT:
			if (*(unsigned int*) m_pArguments[i].pData
					!= *(unsigned int*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_LONG_INT:
			if (*(long*) m_pArguments[i].pData != *(long*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_UNSIGNED_LONG_INT:
			if (*(unsigned long*) m_pArguments[i].pData
					!= *(unsigned long*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_LONG_LONG_INT:
			if (*(long long*) m_pArguments[i].pData
					!= *(long long*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_UNSIGNED_LONG_LONG_INT:
			if (*(unsigned long long*) m_pArguments[i].pData
					!= *(unsigned long long*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_FLOAT:
			if (*(float*) m_pArguments[i].pData != *(float*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_DOUBLE:
			if (*(double*) m_pArguments[i].pData != *(double*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_POINTER:
			if (*(void**) m_pArguments[i].pData != *(void**) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_BOOLEAN:
			if (*(GLboolean*) m_pArguments[i].pData
					!= *(GLboolean*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_BITFIELD:
			if (*(GLbitfield*) m_pArguments[i].pData
					!= *(GLbitfield*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_ENUM:
			if (*(GLenum*) m_pArguments[i].pData != *(GLenum*) rArg->pData) {
				return false;
			}
			break;
		case DBG_TYPE_STRUCT:
			return false; /* FIXME */
			break;
		default:
			return false;
		}
	}
	return true;
}

bool FunctionCall::operator!=(const FunctionCall &right)
{
	return !operator==(right);
}

char* FunctionCall::getArgumentString(Argument arg) const
{
	char *argString, *s;

	switch (arg.iType) {
	case DBG_TYPE_CHAR:
		asprintf(&argString, "%i", *(char*) arg.pData);
		break;
	case DBG_TYPE_UNSIGNED_CHAR:
		asprintf(&argString, "%i", *(unsigned char*) arg.pData);
		break;
	case DBG_TYPE_SHORT_INT:
		asprintf(&argString, "%i", *(short*) arg.pData);
		break;
	case DBG_TYPE_UNSIGNED_SHORT_INT:
		asprintf(&argString, "%i", *(unsigned short*) arg.pData);
		break;
	case DBG_TYPE_INT:
		asprintf(&argString, "%i", *(int*) arg.pData);
		break;
	case DBG_TYPE_UNSIGNED_INT:
		asprintf(&argString, "%u", *(unsigned int*) arg.pData);
		break;
	case DBG_TYPE_LONG_INT:
		asprintf(&argString, "%li", *(long*) arg.pData);
		break;
	case DBG_TYPE_UNSIGNED_LONG_INT:
		asprintf(&argString, "%lu", *(unsigned long*) arg.pData);
		break;
	case DBG_TYPE_LONG_LONG_INT:
		asprintf(&argString, "%lli", *(long long*) arg.pData);
		break;
	case DBG_TYPE_UNSIGNED_LONG_LONG_INT:
		asprintf(&argString, "%llu", *(unsigned long long*) arg.pData);
		break;
	case DBG_TYPE_FLOAT:
		asprintf(&argString, "%f", *(float*) arg.pData);
		break;
	case DBG_TYPE_DOUBLE:
		asprintf(&argString, "%f", *(double*) arg.pData);
		break;
	case DBG_TYPE_POINTER:
		asprintf(&argString, "%p", *(void**) arg.pData);
		break;
	case DBG_TYPE_BOOLEAN:
		asprintf(&argString, "%s", *(GLboolean*) arg.pData ? "TRUE" : "FALSE");
		break;
	case DBG_TYPE_BITFIELD:
		s = dissectBitfield(*(GLbitfield*) arg.pData);
		asprintf(&argString, "%s", s);
		free(s);
		break;
	case DBG_TYPE_ENUM:
		asprintf(&argString, "%s", lookupEnum(*(GLenum*) arg.pData));
		break;
	case DBG_TYPE_STRUCT:
		asprintf(&argString, "STRUCT");
		break;
	default:
		asprintf(&argString, "UNKNOWN_TYPE[%i]", arg.iType);
		break;
	}
	return argString;
}

char* FunctionCall::getCallString(void) const
{
	int i;
	char *callString = (char*) malloc(4096);

	strcpy(callString, m_pName);
	strcat(callString, "(");
	for (i = 0; i < m_iNumArgs; i++) {
		char *argstr = getArgumentString(m_pArguments[i]);
		strcat(callString, argstr);
		free(argstr);
		if (i < m_iNumArgs - 1) {
			strcat(callString, ", ");
		}
	}
	strcat(callString, ")");

	return callString;
}

bool FunctionCall::isDebuggable() const
{
	/* So far we restrict it to draw calls */
	if (isDebuggableDrawCall())
		return true;

	return false;
}

bool FunctionCall::isDebuggable(int *primitiveMode) const
{
	/* So far we restrict it to draw calls */
	if (isDebuggableDrawCall(primitiveMode))
		return true;

	return false;
}

bool FunctionCall::isDebuggableDrawCall(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && f->isDebuggableDrawCall)
		return true;
	return false;
}

bool FunctionCall::isDebuggableDrawCall(int *primitiveMode) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && f->isDebuggableDrawCall) {
		int idx = f->primitiveModeIndex;
		*primitiveMode = *(GLenum*) m_pArguments[idx].pData;
		return true;
	}
	*primitiveMode = GL_NONE;
	return false;
}

bool FunctionCall::isEditable(void) const
{
	return (0 < m_iNumArgs);
}

bool FunctionCall::isShaderSwitch(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && f->isShaderSwitch)
		return true;
	return false;
}

bool FunctionCall::isGlFunc(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && strcmp("GL", f->prefix) == 0)
		return true;
	return false;
}

bool FunctionCall::isGlxFunc(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && strcmp("GLX", f->prefix) == 0)
		return true;
	return false;
}

bool FunctionCall::isWglFunc(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && strcmp("WGL", f->prefix) == 0)
		return true;
	return false;
}

bool FunctionCall::isFrameEnd(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && f->isFrameEnd)
		return true;
	return false;
}

bool FunctionCall::isFramebufferChange(void) const
{
	GLFunctionList *f = FunctionsMap::instance()[m_pName];
	if (f && f->isFramebufferChange)
		return true;
	return false;
}

