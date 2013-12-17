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

#include "postExecution.h"
#include "dbgprint.h"
#include "queries.h"

extern Globals G;

void glGetQueryObjectiv_POSTEXECUTE(GLuint *id, GLenum *pname, GLint **params,
		GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjectiv_POSTEXECUTE %u %i %i error:%i\n", *id, *pname, **params, *error);
}

void glGetQueryObjectuiv_POSTEXECUTE(GLuint *id, GLenum *pname, GLuint **params,
		GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjectuiv_POSTEXECUTE %u %i %u error:%i\n", *id, *pname, **params, *error);
}

void glGetQueryObjectivARB_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLint **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjectivARB_POSTEXECUTE %u %i %i error:%i\n", *id, *pname, **params, *error);
}

void glGetQueryObjectuivARB_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLuint **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjectuivARB_POSTEXECUTE %u %i %u error:%i\n", *id, *pname, **params, *error);
}

void glGetOcclusionQueryivNV_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLint **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetOcclusionQueryivNV_POSTEXECUTE %u %i %i error:%i\n", *id, *pname, **params, *error);
}

void glGetOcclusionQueryuivNV_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLuint **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetOcclusionQueryuivNV_POSTEXECUTE %u %i %u error:%i\n", *id, *pname, **params, *error);
}

void glGetQueryObjecti64vEXT_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLint64EXT **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjecti64vEXT_POSTEXECUTE %u %i %li error:%i\n", *id, *pname, **params, *error);
}

void glGetQueryObjectui64vEXT_POSTEXECUTE(GLuint *id, GLenum *pname,
		GLuint64EXT **params, GLint *error)
{
	Query *q = (Query*) hash_find(&G.queries, id);
	if (q != NULL) {
		**params += q->value;
	}
	dbgPrint(DBGLVL_INFO,
			"glGetQueryObjecti64vEXT_POSTEXECUTE %u %i %lu error:%i\n", *id, *pname, **params, *error);
}

