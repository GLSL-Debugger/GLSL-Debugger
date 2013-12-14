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
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */
#include "../GL/gl.h"
#include "../GL/glext.h"

#include "streamRecording.h"
#include "dbgprint.h"

static int getTypeSize(GLenum type)
{
	switch (type) {
	case GL_BYTE:
		return sizeof(GLbyte);
	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);
	case GL_SHORT:
		return sizeof(GLshort);
	case GL_UNSIGNED_SHORT:
		return sizeof(GLushort);
	case GL_INT:
		return sizeof(GLint);
	case GL_UNSIGNED_INT:
		return sizeof(GLuint);
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_DOUBLE:
		return sizeof(GLdouble);
	case GL_2_BYTES:
		return 2 * sizeof(GLbyte);
	case GL_3_BYTES:
		return 3 * sizeof(GLbyte);
	case GL_4_BYTES:
		return 4 * sizeof(GLbyte);
	default:
		dbgPrint(DBGLVL_INFO, "size of unkown type requested %i\n", type);
		exit(1);
	}
}

int glCallLists_getArg2Size(GLsizei n, GLenum type, const GLvoid *lists)
{
	return n * getTypeSize(type);
}

/*  */
int glEdgeFlagv_getArg0Size(const GLboolean *flag)
{
	return sizeof(GLboolean);
}

/*  */
int glIndexdv_getArg0Size(const GLdouble *c)
{
	return sizeof(GLdouble);
}

/*  */
int glIndexfv_getArg0Size(const GLfloat *c)
{
	return sizeof(GLfloat);
}

/*  */
int glIndexiv_getArg0Size(const GLint *c)
{
	return sizeof(GLint);
}

/*  */
int glIndexsv_getArg0Size(const GLshort *c)
{
	return sizeof(GLshort);
}

/*  */
int glIndexubv_getArg0Size(const GLubyte *c)
{
	return sizeof(GLubyte);
}

/*  */
int glMaterialfv_getArg2Size(GLenum face, GLenum pname, const GLfloat *params)
{
	switch (pname) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_EMISSION:
		return 4 * sizeof(GLfloat);
	case GL_SHININESS:
		return sizeof(GLfloat);
	case GL_AMBIENT_AND_DIFFUSE:
		return 4 * sizeof(GLfloat);
	case GL_COLOR_INDEXES:
		return 3 * sizeof(GLfloat);
	default:
		dbgPrint(DBGLVL_WARNING, "glMaterialfv_getArg2Size: invalid pname\n");
		exit(1);
	}
}

/*  */
int glMaterialiv_getArg2Size(GLenum face, GLenum pname, const GLint *params)
{
	switch (pname) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_EMISSION:
		return 4 * sizeof(GLint);
	case GL_SHININESS:
		return sizeof(GLint);
	case GL_AMBIENT_AND_DIFFUSE:
		return 4 * sizeof(GLint);
	case GL_COLOR_INDEXES:
		return 3 * sizeof(GLint);
	default:
		dbgPrint(DBGLVL_WARNING, "glMaterialfv_getArg2Size: invalid pname\n");
		exit(1);
	}
}

/* GL_VERSION_1_1 */
int glDrawElements_getArg3Size(GLenum mode, GLsizei count, GLenum type,
		const GLvoid *indices)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
		return count * sizeof(GLubyte);
	case GL_UNSIGNED_SHORT:
		return count * sizeof(GLushort);
	case GL_UNSIGNED_INT:
		return count * sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glDrawElements_getArg3Size: invalid type %i\n", mode);
		exit(1);
	}
}

/* GL_VERSION_1_2 */
int glDrawRangeElements_getArg5Size(GLenum mode, GLuint start, GLuint end,
		GLsizei count, GLenum type, const GLvoid *indices)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
		return count * sizeof(GLubyte);
	case GL_UNSIGNED_SHORT:
		return count * sizeof(GLushort);
	case GL_UNSIGNED_INT:
		return count * sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glDrawRangeElements_getArg5Size: invalid type %i\n", mode);
		exit(1);
	}
}

/* GL_VERSION_1_4 */
int glMultiDrawArrays_getArg1Size(GLenum mode, GLint *first, GLsizei *count,
		GLsizei primcount)
{
	return primcount * sizeof(GLint);
}

/* GL_VERSION_1_4 */
int glMultiDrawArrays_getArg2Size(GLenum mode, GLint *first, GLsizei *count,
		GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_VERSION_1_4 */
int glMultiDrawElements_getArg1Size(GLenum mode, const GLsizei *count,
		GLenum type, const GLvoid **indices, GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_VERSION_1_4 */
int glMultiDrawElements_getArg3Size(GLenum mode, const GLsizei *count,
		GLenum type, const GLvoid **indices, GLsizei primcount)
{
	/* WARNING: since indices holds pointers to index list this will work only
	 * for draw call recording. When recording a general OpenGL stream this will
	 * fail when the data in this lists is modified!
	 */
	return primcount * sizeof(GLvoid*);
}

/* GL_EXT_draw_range_elements*/
int glDrawRangeElementsEXT_getArg5Size(GLenum mode, GLuint start, GLuint end,
		GLsizei count, GLenum type, const GLvoid *indices)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
		return count * sizeof(GLubyte);
	case GL_UNSIGNED_SHORT:
		return count * sizeof(GLushort);
	case GL_UNSIGNED_INT:
		return count * sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glDrawRangeElementsEXT_getArg3Size: invalid type %i\n", mode);
		exit(1);
	}
}

/* GL_EXT_multi_draw_arrays */
int glMultiDrawArraysEXT_getArg1Size(GLenum mode, GLint *first, GLsizei *count,
		GLsizei primcount)
{
	return primcount * sizeof(GLint);
}

/* GL_EXT_multi_draw_arrays */
int glMultiDrawArraysEXT_getArg2Size(GLenum mode, GLint *first, GLsizei *count,
		GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_EXT_multi_draw_arrays */
int glMultiDrawElementsEXT_getArg1Size(GLenum mode, const GLsizei *count,
		GLenum type, const GLvoid **indices, GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_EXT_multi_draw_arrays */
int glMultiDrawElementsEXT_getArg3Size(GLenum mode, const GLsizei *count,
		GLenum type, const GLvoid ** indices, GLsizei primcount)
{
	/* WARNING: since indices holds pointers to index list this will work only
	 * for draw call recording. When recording a general OpenGL stream this will
	 * fail when the data in this lists is modified!
	 */
	return primcount * sizeof(GLvoid*);
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawArraysIBM_getArg0Size(const GLenum *mode, const GLint *first,
		const GLsizei *count, GLsizei primcount, GLint modestride)
{
	return primcount * sizeof(GLenum);
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawArraysIBM_getArg1Size(const GLenum *mode, const GLint *first,
		const GLsizei *count, GLsizei primcount, GLint modestride)
{
	return primcount * sizeof(GLint);
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawArraysIBM_getArg2Size(const GLenum *mode, const GLint *first,
		const GLsizei *count, GLsizei primcount, GLint modestride)
{
	return primcount * sizeof(GLsizei);
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawElementsIBM_getArg0Size(const GLenum *mode,
		const GLsizei *count, GLenum type, const GLvoid* const *indices,
		GLsizei primcount, GLint modestride)
{
	if (primcount == 0) {
		return 0;
	} else {
		return (primcount - 1) * modestride + sizeof(GLenum);
	}
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawElementsIBM_getArg1Size(const GLenum *mode,
		const GLsizei *count, GLenum type, const GLvoid* const *indices,
		GLsizei primcount, GLint modestride)
{
	return primcount * sizeof(GLsizei);
}

/* GL_IBM_multimode_draw_arrays */
int glMultiModeDrawElementsIBM_getArg3Size(const GLenum *mode,
		const GLsizei *count, GLenum type, const GLvoid* const *indices,
		GLsizei primcount, GLint modestride)
{
	/* WARNING: since indices holds pointers to index list this will work only
	 * for draw call recording. When recording a general OpenGL stream this will
	 * fail when the data in this lists is modified!
	 */
	return primcount * sizeof(GLvoid*);
}

/* GL_APPLE_element_array */
int glMultiDrawElementArrayAPPLE_getArg1Size(GLenum mode, const GLint *first,
		const GLsizei *count, GLsizei primcount)
{
	return primcount * sizeof(GLint);
}

/* GL_APPLE_element_array */
int glMultiDrawElementArrayAPPLE_getArg2Size(GLenum mode, const GLint *first,
		const GLsizei *count, GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_APPLE_element_array */
int glMultiDrawRangeElementArrayAPPLE_getArg3Size(GLenum mode, GLuint start,
		GLuint end, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	return primcount * sizeof(GLint);
}

/* GL_APPLE_element_array */
int glMultiDrawRangeElementArrayAPPLE_getArg4Size(GLenum mode, GLuint start,
		GLuint end, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	return primcount * sizeof(GLsizei);
}

/* GL_EXT_draw_instanced */
int glDrawElementsInstancedEXT_getArg3Size(GLenum mode, GLsizei count,
		GLenum type, const GLvoid *indices, GLsizei primcount)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
		return count * sizeof(GLubyte);
	case GL_UNSIGNED_SHORT:
		return count * sizeof(GLushort);
	case GL_UNSIGNED_INT:
		return count * sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glDrawElementsInstancedEXT_getArg3Size: invalid type %i\n", mode);
		exit(1);
	}
}

/* GL_VERSION_1_4 */
int glFogCoordfv_getArg0Size(const GLfloat *coord)
{
	return sizeof(GLfloat);
}

/* GL_VERSION_1_4 */
int glFogCoorddv_getArg0Size(const GLdouble *coord)
{
	return sizeof(GLdouble);
}

/* GL_ARB_vertex_blend */
int glWeightbvARB_getArg1Size(GLint size, const GLbyte *weights)
{
	return size * sizeof(GLbyte);
}

/* GL_ARB_vertex_blend */
int glWeightsvARB_getArg1Size(GLint size, const GLshort *weights)
{
	return size * sizeof(GLshort);
}

/* GL_ARB_vertex_blend */
int glWeightivARB_getArg1Size(GLint size, const GLint *weights)
{
	return size * sizeof(GLint);
}

/* GL_ARB_vertex_blend */
int glWeightfvARB_getArg1Size(GLint size, const GLfloat *weights)
{
	return size * sizeof(GLfloat);
}

/* GL_ARB_vertex_blend */
int glWeightdvARB_getArg1Size(GLint size, const GLdouble *weights)
{
	return size * sizeof(GLdouble);
}

/* GL_ARB_vertex_blend */
int glWeightubvARB_getArg1Size(GLint size, const GLubyte *weights)
{
	return size * sizeof(GLubyte);
}

/* GL_ARB_vertex_blend */
int glWeightusvARB_getArg1Size(GLint size, const GLushort *weights)
{
	return size * sizeof(GLushort);
}

/* GL_ARB_vertex_blend */
int glWeightuivARB_getArg1Size(GLint size, const GLuint *weights)
{
	return size * sizeof(GLuint);
}

/* GL_ARB_matrix_palette */
int glMatrixIndexubvARB_getArg1Size(GLint size, const GLubyte *indices)
{
	return size * sizeof(GLubyte);
}

/* GL_ARB_matrix_palette */
int glMatrixIndexusvARB_getArg1Size(GLint size, const GLushort *indices)
{
	return size * sizeof(GLushort);
}

/* GL_ARB_matrix_palette */
int glMatrixIndexuivARB_getArg1Size(GLint size, const GLuint *indices)
{
	return size * sizeof(GLuint);
}

/* GL_EXT_fog_coord */
int glFogCoordfvEXT_getArg0Size(const GLfloat *coord)
{
	return sizeof(GLfloat);
}

/* GL_EXT_fog_coord */
int glFogCoorddvEXT_getArg0Size(const GLdouble *coord)
{
	return sizeof(GLdouble);
}

/* GL_SUN_triangle_list */
int glReplacementCodeuivSUN_getArg0Size(const GLuint *code)
{
	return sizeof(GLuint);
}

/* GL_SUN_triangle_list */
int glReplacementCodeusvSUN_getArg0Size(const GLushort *code)
{
	return sizeof(GLushort);
}

/* GL_SUN_triangle_list */
int glReplacementCodeubvSUN_getArg0Size(const GLubyte *code)
{
	return sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glColor4ubVertex2fvSUN_getArg0Size(const GLubyte *c, const GLfloat *v)
{
	return 4 * sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glColor4ubVertex2fvSUN_getArg1Size(const GLubyte *c, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor4ubVertex3fvSUN_getArg0Size(const GLubyte *c, const GLfloat *v)
{
	return 4 * sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glColor4ubVertex3fvSUN_getArg1Size(const GLubyte *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor3fVertex3fvSUN_getArg0Size(const GLfloat *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor3fVertex3fvSUN_getArg1Size(const GLfloat *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glNormal3fVertex3fvSUN_getArg0Size(const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glNormal3fVertex3fvSUN_getArg1Size(const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor4fNormal3fVertex3fvSUN_getArg0Size(const GLfloat *c,
		const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor4fNormal3fVertex3fvSUN_getArg1Size(const GLfloat *c,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glColor4fNormal3fVertex3fvSUN_getArg2Size(const GLfloat *c,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fVertex3fvSUN_getArg0Size(const GLfloat *tc, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fVertex3fvSUN_getArg1Size(const GLfloat *tc, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fVertex4fvSUN_getArg0Size(const GLfloat *tc, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fVertex4fvSUN_getArg1Size(const GLfloat *tc, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4ubVertex3fvSUN_getArg0Size(const GLfloat *tc,
		const GLubyte *c, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4ubVertex3fvSUN_getArg1Size(const GLfloat *tc,
		const GLubyte *c, const GLfloat *v)
{
	return 4 * sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4ubVertex3fvSUN_getArg2Size(const GLfloat *tc,
		const GLubyte *c, const GLfloat *v)
{
	return 3 * sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glTexCoord2fColor3fVertex3fvSUN_getArg0Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor3fVertex3fvSUN_getArg1Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor3fVertex3fvSUN_getArg2Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fNormal3fVertex3fvSUN_getArg0Size(const GLfloat *tc,
		const GLfloat *n, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fNormal3fVertex3fvSUN_getArg1Size(const GLfloat *tc,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fNormal3fVertex3fvSUN_getArg2Size(const GLfloat *tc,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4fNormal3fVertex3fvSUN_getArg0Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4fNormal3fVertex3fvSUN_getArg1Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4fNormal3fVertex3fvSUN_getArg2Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord2fColor4fNormal3fVertex3fvSUN_getArg3Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fColor4fNormal3fVertex4fvSUN_getArg0Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fColor4fNormal3fVertex4fvSUN_getArg1Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fColor4fNormal3fVertex4fvSUN_getArg2Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glTexCoord4fColor4fNormal3fVertex4fvSUN_getArg3Size(const GLfloat *tc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4ubVertex3fvSUN_getArg0Size(const GLuint *rc,
		const GLubyte *c, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4ubVertex3fvSUN_getArg1Size(const GLuint *rc,
		const GLubyte *c, const GLfloat *v)
{
	return 4 * sizeof(GLubyte);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4ubVertex3fvSUN_getArg2Size(const GLuint *rc,
		const GLubyte *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor3fVertex3fvSUN_getArg0Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor3fVertex3fvSUN_getArg1Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor3fVertex3fvSUN_getArg2Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiNormal3fVertex3fvSUN_getArg0Size(const GLuint *rc,
		const GLfloat *n, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiNormal3fVertex3fvSUN_getArg1Size(const GLuint *rc,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiNormal3fVertex3fvSUN_getArg2Size(const GLuint *rc,
		const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4fNormal3fVertex3fvSUN_getArg0Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4fNormal3fVertex3fvSUN_getArg1Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4fNormal3fVertex3fvSUN_getArg2Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiColor4fNormal3fVertex3fvSUN_getArg3Size(const GLuint *rc,
		const GLfloat *c, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fVertex3fvSUN_getArg0Size(const GLuint *rc,
		const GLfloat *tc, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fVertex3fvSUN_getArg1Size(const GLuint *rc,
		const GLfloat *tc, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fVertex3fvSUN_getArg2Size(const GLuint *rc,
		const GLfloat *tc, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN_getArg0Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *n, const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN_getArg1Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *n, const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN_getArg2Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN_getArg3Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *n, const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN_getArg0Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *c, const GLfloat *n,
		const GLfloat *v)
{
	return sizeof(GLuint);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN_getArg1Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *c, const GLfloat *n,
		const GLfloat *v)
{
	return 2 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN_getArg2Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *c, const GLfloat *n,
		const GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN_getArg3Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *c, const GLfloat *n,
		const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_SUN_vertex */
int glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN_getArg4Size(
		const GLuint *rc, const GLfloat *tc, const GLfloat *c, const GLfloat *n,
		const GLfloat *v)
{
	return 3 * sizeof(GLfloat);
}

/* GL_EXT_vertex_weighting */
int glVertexWeightfvEXT_getArg0Size(const GLfloat *weight)
{
	return sizeof(GLfloat);
}

/* GL_ATI_fragment_shader */
int glSetFragmentShaderConstantATI_getArg1Size(GLuint dst, const GLfloat *value)
{
	return 4 * sizeof(GLfloat);
}

/* GL_EXT_vertex_shader */
int glSetInvariantEXT_getArg2Size(GLuint id, GLenum type, const GLvoid *addr)
{
	return getTypeSize(type);
}

/* GL_EXT_vertex_shader */
int glSetLocalConstantEXT_getArg2Size(GLuint id, GLenum type,
		const GLvoid *addr)
{
	return getTypeSize(type);
}

/* GL_EXT_vertex_shader */
int glVariantbvEXT_getArg1Size(GLuint id, const GLbyte *addr)
{
	return sizeof(GLbyte);
}

/* GL_EXT_vertex_shader */
int glVariantsvEXT_getArg1Size(GLuint id, const GLshort *addr)
{
	return sizeof(GLshort);
}

/* GL_EXT_vertex_shader */
int glVariantivEXT_getArg1Size(GLuint id, const GLint *addr)
{
	return sizeof(GLint);
}

/* GL_EXT_vertex_shader */
int glVariantfvEXT_getArg1Size(GLuint id, const GLfloat *addr)
{
	return sizeof(GLfloat);
}

/* GL_EXT_vertex_shader */
int glVariantdvEXT_getArg1Size(GLuint id, const GLdouble *addr)
{
	return sizeof(GLdouble);
}

/* GL_EXT_vertex_shader */
int glVariantubvEXT_getArg1Size(GLuint id, const GLubyte *addr)
{
	return sizeof(GLubyte);
}

/* GL_EXT_vertex_shader */
int glVariantusvEXT_getArg1Size(GLuint id, const GLushort *addr)
{
	return sizeof(GLushort);
}

/* GL_EXT_vertex_shader */
int glVariantuivEXT_getArg1Size(GLuint id, const GLuint *addr)
{
	return sizeof(GLuint);
}

/* GL_NV_point_sprite */
int glPointParameterivNV_getArg1Size(GLenum pname, const GLint *params)
{
	return sizeof(GLint);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4fNV_getArg2Size(GLuint id, GLsizei len,
		const GLubyte *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	return len * sizeof(GLubyte);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4dNV_getArg2Size(GLuint id, GLsizei len,
		const GLubyte *name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	return len * sizeof(GLubyte);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4fvNV_getArg2Size(GLuint id, GLsizei len,
		const GLubyte *name, GLfloat *v)
{
	return len * sizeof(GLubyte);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4fvNV_getArg3Size(GLuint id, GLsizei len,
		const GLubyte *name, GLfloat *v)
{
	return 4 * sizeof(GLfloat);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4dvNV_getArg2Size(GLuint id, GLsizei len,
		const GLubyte *name, GLdouble *v)
{
	return len * sizeof(GLubyte);
}

/* GL_NV_fragment_program */
int glProgramNamedParameter4dvNV_getArg3Size(GLuint id, GLsizei len,
		const GLubyte *name, GLdouble *v)
{
	return 4 * sizeof(GLdouble);
}

/* GL_NV_half_float */
int glFogCoordhvNV_getArg0Size(const GLhalfNV *weight)
{
	return sizeof(GLhalfNV);
}

/* GL_NV_half_float */
int glVertexWeighthvNV_getArg0Size(const GLhalfNV *weight)
{
	return sizeof(GLhalfNV);;
}

/* GL_NV_parameter_buffer_object */
int glProgramBufferParametersfvNV_getArg4Size(GLenum target, GLuint buffer,
		GLuint index, GLsizei count, const GLfloat *params)
{
	return count * sizeof(GLfloat);
}

/* GL_NV_parameter_buffer_object */
int glProgramBufferParametersIivNV_getArg4Size(GLenum target, GLuint buffer,
		GLuint index, GLsizei count, const GLint *params)
{
	return count * sizeof(GLint);
}

/* GL_NV_parameter_buffer_object */
int glProgramBufferParametersIuivNV_getArg4Size(GLenum target, GLuint buffer,
		GLuint index, GLsizei count, const GLuint *params)
{
	return count * sizeof(GLuint);
}

/* GL_VERSION_3_0 */
int glClearBufferiv_getArg2Size(GLenum buffer, GLint drawBuffer,
		const GLint *value)
{
	switch (buffer) {
	case GL_COLOR:
		return 4 * sizeof(GLint);
	case GL_DEPTH:
	case GL_STENCIL:
		return sizeof(GLint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glClearBufferiv_getArg2Size: invalid type %i\n", buffer);
		exit(1);
	}
}

/* GL_VERSION_3_0 */
int glClearBufferuiv_getArg2Size(GLenum buffer, GLint drawBuffer,
		const GLuint *value)
{
	switch (buffer) {
	case GL_COLOR:
		return 4 * sizeof(GLuint);
	case GL_DEPTH:
	case GL_STENCIL:
		return sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glClearBufferuiv_getArg2Size: invalid type %i\n", buffer);
		exit(1);
	}
}

/* GL_VERSION_3_0 */
int glClearBufferfv_getArg2Size(GLenum buffer, GLint drawBuffer,
		const GLfloat *value)
{
	switch (buffer) {
	case GL_COLOR:
		return 4 * sizeof(GLfloat);
	case GL_DEPTH:
	case GL_STENCIL:
		return sizeof(GLfloat);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glClearBufferfv_getArg2Size: invalid type %i\n", buffer);
		exit(1);
	}
}

/* GL_VERSION_3_0 */
int glDrawElementsInstanced_getArg3Size(GLenum mode, GLsizei count, GLenum type,
		const GLvoid *indices, GLsizei primcount)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
		return count * sizeof(GLubyte);
	case GL_UNSIGNED_SHORT:
		return count * sizeof(GLushort);
	case GL_UNSIGNED_INT:
		return count * sizeof(GLuint);
	default:
		dbgPrint(DBGLVL_WARNING,
				"glDrawElementsInstanced_getArg3Size: invalid type %i\n", mode);
		exit(1);
	}
}

