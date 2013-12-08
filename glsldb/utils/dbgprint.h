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

#ifndef _DBGPRINT_H
#define _DBGPRINT_H
#if (_MSC_VER > 1000)
#pragma once
#endif /* (_MSC_VER > 1000) */

#include "../DebugLib/debuglibExport.h"

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void) x;
#endif

/**
 * Output debug messages on stderr and on Windows additionally on the
 * output window of the attached debugger.
 */
#ifdef __cplusplus
extern "C" {
#endif /* _CPP */

enum DBG_LEVELS {
	DBGLVL_ERROR = 0,
	DBGLVL_WARNING,
	DBGLVL_INFO,
	DBGLVL_INTERNAL_WARNING,
	DBGLVL_COMPILERINFO,
	DBGLVL_DEBUG
};

#if defined DEBUG
#  define dbgPrint(LEVEL, ...) \
    _dbgPrint_(LEVEL, 1, __VA_ARGS__)
#  define dbgPrintNoPrefix(LEVEL, ...) \
    _dbgPrint_(LEVEL, 0, __VA_ARGS__)
#else
#  define dbgPrint(LEVEL, ...) \
    ((void)(((LEVEL) < DBGLVL_DEBUG) ? _dbgPrint_(LEVEL, 1, __VA_ARGS__) : 0))
#  define dbgPrintNoPrefix(LEVEL, ...) \
    ((void)(((LEVEL) < DBGLVL_DEBUG) ? _dbgPrint_(LEVEL, 0, __VA_ARGS__) : 0))
#endif

DBGLIBLOCAL void setMaxDebugOutputLevel(int level);

DBGLIBLOCAL int getMaxDebugOutputLevel(void);

DBGLIBLOCAL void setLogDir(const char *dirName);

DBGLIBLOCAL const char* getLogDir();

DBGLIBLOCAL void startLogging(const char *baseName);
DBGLIBLOCAL void quitLogging();

DBGLIBLOCAL int _dbgPrint_(int level, int printPrefix, const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* _CPP */

#endif /* _DBGPRINT_H */
