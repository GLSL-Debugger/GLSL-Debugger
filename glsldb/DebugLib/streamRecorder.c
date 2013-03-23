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
#include <string.h>
#include <stdarg.h>

#include "streamRecorder.h"
#include "replayFunction.h"
#include "../utils/dbgprint.h"

void initStreamRecorder(StreamRecorder *rec)
{
	rec->numCalls = 0;
	rec->calls = NULL;
	rec->lastCall = NULL;
}

void recordFunctionCall(StreamRecorder *rec, const char *fname, int numArgs, ...)
{
	int i;
	va_list argp;
	StoredCall *newCall;
	
	dbgPrint(DBGLVL_INFO, "RECORD CALL: %s\n", fname);

	rec->numCalls++;
	newCall =  malloc(sizeof(StoredCall));
	if (!newCall) {
		dbgPrint(DBGLVL_ERROR, "Allocation of recorded call failed\n");
		exit(1); /* TODO: proper error handling */
	}
	newCall->fname = strdup(fname);
	newCall->numArguments = numArgs;
	newCall->arguments = malloc(numArgs*sizeof(sizeof(void*)));
	if (!newCall->fname || !newCall->arguments) {
		dbgPrint(DBGLVL_ERROR, "Allocation of recorded call failed\n");
		exit(1); /* TODO: proper error handling */
	}
	va_start(argp, numArgs);
	for (i = 0; i < numArgs; i++) {
		void *ptr = (void*)va_arg(argp, void*);
		int size = (int)va_arg(argp, int);
		newCall->arguments[i] = malloc(size);
		if (!newCall->arguments[i]) {
			dbgPrint(DBGLVL_ERROR, "Allocation of recorded call failed\n");
			exit(1); /* TODO: proper error handling */
		}
		memcpy(newCall->arguments[i], ptr, size);
	}	
	va_end(argp);
	if (rec->calls) {
		rec->lastCall->nextCall = newCall;
	} else {
		rec->calls = newCall;
	}
	rec->lastCall = newCall;
}


void replayFunctionCalls(StreamRecorder *rec, int final)
{
	int i;
	StoredCall *call = rec->calls;

	for (i = 0; i < rec->numCalls; i++) {
		replayFunctionCall(call, final);
		call = call->nextCall;
	}
}

void clearRecordedCalls(StreamRecorder *rec)
{
	int i, j;
	
	StoredCall *sc =  rec->calls;
	for (i = 0; i < rec->numCalls; i++) {
		StoredCall *current = sc;
		free(current->fname);
		for (j = 0; j < current->numArguments; j++) {
			free(current->arguments[j]);
		}
		free(current->arguments);
		sc = current->nextCall;
		free(current);
	}
	rec->numCalls = 0;
	rec->calls = NULL;
	rec->lastCall = NULL;
}

