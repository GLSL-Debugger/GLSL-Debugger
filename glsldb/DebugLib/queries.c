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
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif /* !_WIN32 */
#include <string.h>

#include "debuglib.h"
#include "debuglibInternal.h"
#include "queries.h"
#include "glenumerants.h"
#include "dbgprint.h"

#ifdef _WIN32
#include "generated/trampolines.h"
#endif /* _WIN32 */

extern Globals G;

static int hashUInt(const void *key, int numBuckets)
{
	return *(GLuint*) key % numBuckets;
}

static int compUInt(const void *key1, const void *key2)
{
	return *(GLuint*) key1 == *(GLuint*) key2;
}

int initQueryStateTracker(void)
{
	hash_create(&G.queries, hashUInt, compUInt, 128, 1);
	return DBG_NO_ERROR;
}

int cleanupQueryStateTracker(void)
{
	hash_free(&G.queries);
	return DBG_NO_ERROR;
}

void interruptAndSaveQueries(void)
{
	Query *activeQuery;
	GLint qid;
	GLint value;
	int i, n;

	dbgPrint(DBGLVL_INFO, "interruptAndSaveQueries called\n");
	/* reset state of hashed but not restarted queries */
	n = hash_count(&G.queries);
	for (i = 0; i < n; i++) {
		Query *q = hash_element(&G.queries, i);
		q->interrupted = 0;
	}

	if (checkGLVersionSupported(1, 5)) {
		/* check occlusion query */
		ORIG_GL(glGetQueryiv)(GL_SAMPLES_PASSED, GL_CURRENT_QUERY, &qid);
		if (qid != 0) {
			ORIG_GL(glEndQuery)(GL_SAMPLES_PASSED);
			ORIG_GL(glGetQueryObjectiv)(qid, GL_QUERY_RESULT, &value);
			if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
				setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
				return;
			}
			activeQuery->id = qid;
			activeQuery->value = value;
			activeQuery->target = GL_SAMPLES_PASSED;
			activeQuery->interrupted = 1;
			dbgPrint(DBGLVL_INFO,
					"interruptAndSaveQuery: id=%i target=%s value=%i\n", activeQuery->id, lookupEnum(activeQuery->target), activeQuery->value);
			hash_insert(&G.queries, &activeQuery->id, activeQuery);
		}
		/* check timer query??? */
		/* check tfb queries */
		switch (getTFBVersion()) {
		case TFBVersion_NV:
			ORIG_GL(glGetQueryiv)(GL_PRIMITIVES_GENERATED_NV, GL_CURRENT_QUERY,
					&qid);
			if (qid != 0) {
				ORIG_GL(glEndQuery)(GL_PRIMITIVES_GENERATED_NV);
				ORIG_GL(glGetQueryObjectiv)(qid, GL_QUERY_RESULT, &value);
				if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
					setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
					return;
				}
				activeQuery->id = qid;
				activeQuery->value = value;
				activeQuery->target = GL_PRIMITIVES_GENERATED_NV;
				activeQuery->interrupted = 1;
				hash_insert(&G.queries, &activeQuery->id, activeQuery);
			}
			ORIG_GL(glGetQueryiv)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV,
					GL_CURRENT_QUERY, &qid);
			if (qid != 0) {
				ORIG_GL(glEndQuery)(
						GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV);
				ORIG_GL(glGetQueryObjectiv)(qid, GL_QUERY_RESULT, &value);
				if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
					setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
					return;
				}
				activeQuery->id = qid;
				activeQuery->value = value;
				activeQuery->target =
						GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV;
				activeQuery->interrupted = 1;
				hash_insert(&G.queries, &activeQuery->id, activeQuery);
			}
			break;
		case TFBVersion_EXT:
			ORIG_GL(glGetQueryiv)(GL_PRIMITIVES_GENERATED_EXT, GL_CURRENT_QUERY,
					&qid);
			if (qid != 0) {
				ORIG_GL(glEndQuery)(GL_PRIMITIVES_GENERATED_EXT);
				ORIG_GL(glGetQueryObjectiv)(qid, GL_QUERY_RESULT, &value);
				if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
					setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
					return;
				}
				activeQuery->id = qid;
				activeQuery->value = value;
				activeQuery->target = GL_PRIMITIVES_GENERATED_EXT;
				activeQuery->interrupted = 1;
				hash_insert(&G.queries, &activeQuery->id, activeQuery);
			}
			ORIG_GL(glGetQueryiv)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT,
					GL_CURRENT_QUERY, &qid);
			if (qid != 0) {
				ORIG_GL(glEndQuery)(
						GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT);
				ORIG_GL(glGetQueryObjectiv)(qid, GL_QUERY_RESULT, &value);
				if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
					setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
					return;
				}
				activeQuery->id = qid;
				activeQuery->value = value;
				activeQuery->target =
						GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT;
				activeQuery->interrupted = 1;
				hash_insert(&G.queries, &activeQuery->id, activeQuery);
			}
			break;
		default:
			/* nothing to do, no transform feedback */
			break;
		}
	} else if (checkGLExtensionSupported("GL_ARB_occlusion_query")) {
		/* check occlusion query */
		ORIG_GL(glGetQueryivARB)(GL_SAMPLES_PASSED_ARB, GL_CURRENT_QUERY_ARB,
				&qid);
		if (qid != 0) {
			ORIG_GL(glEndQueryARB)(GL_SAMPLES_PASSED_ARB);
			ORIG_GL(glGetQueryObjectivARB)(qid, GL_QUERY_RESULT_ARB, &value);
			if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
				setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
				return;
			}
			activeQuery->id = qid;
			activeQuery->value = value;
			activeQuery->target = GL_SAMPLES_PASSED_ARB;
			activeQuery->interrupted = 1;
			hash_insert(&G.queries, &activeQuery->id, activeQuery);
		}
	} else if (checkGLExtensionSupported("GL_NV_occlusion_query")) {
		/* check occlusion query */
		ORIG_GL(glGetIntegerv)(GL_CURRENT_OCCLUSION_QUERY_ID_NV, &qid);
		if (qid != 0) {
			ORIG_GL(glEndOcclusionQueryNV)();
			ORIG_GL(glGetOcclusionQueryivNV)(qid, GL_PIXEL_COUNT_NV, &value);
			if (!(activeQuery = (Query*) malloc(sizeof(Query)))) {
				setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
				return;
			}
			activeQuery->id = qid;
			activeQuery->value = value;
			activeQuery->target = GL_CURRENT_OCCLUSION_QUERY_ID_NV;
			activeQuery->interrupted = 1;
			hash_insert(&G.queries, &activeQuery->id, activeQuery);
		}
	}
	setErrorCode(DBG_NO_ERROR);
}

void restartQueries(void)
{
	int i;
	int n = hash_count(&G.queries);

	dbgPrint(DBGLVL_INFO, "restartQueries: %i\n", n);
	for (i = 0; i < n; i++) {
		Query *q = hash_element(&G.queries, i);
		dbgPrint(DBGLVL_INFO,
				"restarting query %i: id=%i target=%s value=%i interrupted=%i\n", i, q->id, lookupEnum(q->target), q->value, q->interrupted);
		if (q->interrupted) {
			if (checkGLVersionSupported(1, 5)) {
				dbgPrint(DBGLVL_INFO,
						"restarting query target=%s id=%i!\n", lookupEnum(q->target), q->id);
				ORIG_GL(glBeginQuery)(q->target, q->id);
			} else if (checkGLExtensionSupported("GL_ARB_occlusion_query")) {
				ORIG_GL(glBeginQueryARB)(q->target, q->id);
			} else if (checkGLExtensionSupported("GL_NV_occlusion_query")) {
				ORIG_GL(glBeginOcclusionQueryNV)(q->id);
			}
		}
	}
	setErrorCode(DBG_NO_ERROR);
}

