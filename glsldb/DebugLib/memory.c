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

#include <stdio.h>
#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#endif /* _WIN32 */
#include <stdlib.h>

#include "debuglib.h"
#include "debuglibInternal.h"
#include "memory.h"
#include "dbgprint.h"

void allocMem(void)
{
	int i;
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);

	for (i = 0; i < rec->numItems; i++) {
		rec->items[i] = (ALIGNED_DATA) malloc(rec->items[i] * sizeof(char));
		if (!rec->items[i]) {
			dbgPrint(DBGLVL_WARNING,
					"allocMem: Allocation of scratch mem failed\n");
			for (i--; i >= 0; i--) {
				free((void*) rec->items[i]);
			}
			setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
			return;
		}
	}
	rec->result = DBG_ALLOCATED;
}

void freeMem(void)
{
	int i;
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);

	for (i = 0; i < rec->numItems; i++) {
		free((void*) rec->items[i]);
	}
	setErrorCode(DBG_NO_ERROR);
}

