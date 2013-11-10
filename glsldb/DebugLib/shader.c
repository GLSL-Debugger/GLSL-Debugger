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

#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#include <stdlib.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#else /* !_WIN32 */
#include "asprintf.h"
#endif /* !_WIN32 */
#include <errno.h>
#include <string.h>

#include "debuglib.h"
#include "debuglibInternal.h"
#include "../glenumerants/glenumerants.h"
#include "shader.h"
#include "../utils/dbgprint.h"
#include "../utils/notify.h"
#include "ResourceLimits.h"

#ifdef _WIN32
#include "trampolines.h"
#endif /* _WIN32 */

typedef struct {
	GLuint handle;
	GLint type;
	int srcLength;
	char *src;
} ShaderObject;

typedef struct {
	char *name;
	GLint location;
	GLuint type;
	GLint size;
	int builtin;
	void *value;
} ActiveUniform;

typedef struct {
	char *name;
	int location;
	GLuint type;
	GLint size;
	int builtin;
} ActiveAttribute;

typedef struct {
	GLuint programHandle;
	GLint numObjects;
	ShaderObject *objects;
	GLint numUniforms;
	ActiveUniform *uniforms;
	GLint numAttributes;
	ActiveAttribute *attributes;

	/* geometry shader */
	GLint geoVerticesOut;
	GLint geoInputType;
	GLint geoOutputType;
} ShaderProgram;

/* FIXME: not thread-safe! */
static struct {
	ShaderProgram storedShader;
	GLint dbgShaderHandle;
} g = {{0, 0, NULL, 0, NULL, 0, NULL}, -1};

/* TODO TODO TODO Geometry Shader!!!!!!!!!!!!!! */


static int getTypeId(GLint type)
{
    switch (type) {
        case GL_VERTEX_SHADER:
            return 0;
        case GL_GEOMETRY_SHADER_EXT:
            return 1;
        case GL_FRAGMENT_SHADER:
            return 2;
        default:
            UT_NOTIFY_VA(LV_ERROR, "Wow, you found a new shader type %i", type);
            exit(1);
    }
}

int uniformNumElements(ActiveUniform *v)
{
	switch (v->type) {
		case GL_INT:
		case GL_BOOL:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
		case GL_FLOAT:
			return 1;
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_FLOAT_VEC2:
		case GL_UNSIGNED_INT_VEC2_EXT:
			return 2;
			break;
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_FLOAT_VEC3:
		case GL_UNSIGNED_INT_VEC3_EXT:
			return 3;
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 4;
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			return 6;
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT4x2:
			return 8;
		case GL_FLOAT_MAT3:
			return 9;
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
			return 12;
		case GL_FLOAT_MAT4:
			return 16;
		default:
			UT_NOTIFY_VA(LV_ERROR, "unkown shader variable type: %i", v->type);
			return 0;
	}
}

static GLenum shaderTypeSize(GLenum type)
{
	switch (type) {
		case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_FLOAT_VEC2:
			return 2*sizeof(GLfloat);
		case GL_FLOAT_VEC3:
			return 3*sizeof(GLfloat);
		case GL_FLOAT_VEC4:
			return 4*sizeof(GLfloat);
		case GL_FLOAT_MAT2:
			return 4*sizeof(GLfloat);
		case GL_FLOAT_MAT2x3:
			return 6*sizeof(GLfloat);
		case GL_FLOAT_MAT2x4:
			return 8*sizeof(GLfloat);
		case GL_FLOAT_MAT3:
			return 9*sizeof(GLfloat);
		case GL_FLOAT_MAT3x2:
			return 6*sizeof(GLfloat);
		case GL_FLOAT_MAT3x4:
			return 12*sizeof(GLfloat);
		case GL_FLOAT_MAT4:
			return 16*sizeof(GLfloat);
		case GL_FLOAT_MAT4x2:
			return 8*sizeof(GLfloat);
		case GL_FLOAT_MAT4x3:
			return 12*sizeof(GLfloat);
		case GL_INT:
			return sizeof(GLint);
		case GL_INT_VEC2:
			return 2*sizeof(GLint);
		case GL_INT_VEC3:
			return 3*sizeof(GLint);
		case GL_INT_VEC4:
			return 4*sizeof(GLint);
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
			return sizeof(GLint);
		case GL_BOOL:
			return sizeof(GLboolean);
		case GL_BOOL_VEC2:
			return 2*sizeof(GLboolean);
		case GL_BOOL_VEC3:
			return 3*sizeof(GLboolean);
		case GL_BOOL_VEC4:
			return 4*sizeof(GLboolean);
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
		case GL_UNSIGNED_INT_VEC2_EXT:
			return 2*sizeof(GLuint);
		case GL_UNSIGNED_INT_VEC3_EXT:
			return 3*sizeof(GLuint);
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 4*sizeof(GLuint);
		default:
			UT_NOTIFY_VA(LV_ERROR, "unkown shader variable type: %i", type);
			return 0;
	}
}
static GLenum uniformBaseType(ActiveUniform *v)
{
	switch (v->type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			return GL_FLOAT;
		case GL_INT:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
			return GL_INT;
		case GL_BOOL:
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
			return GL_BOOL;
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2_EXT:
		case GL_UNSIGNED_INT_VEC3_EXT:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return GL_UNSIGNED_INT;
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", v->type);
			return GL_BYTE;
	}
}

static int shaderBaseTypeSize(GLenum t)
{
	switch (t) {
		case GL_INT:
			return sizeof(GLint);
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
		case GL_BOOL:
			return sizeof(GLboolean);
		case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_DOUBLE:
			return sizeof(GLdouble);
		case GL_BYTE:
			return sizeof(GLbyte);
		case GL_UNSIGNED_BYTE:
			return sizeof(GLubyte);
		case GL_SHORT:
			return sizeof(GLshort);
		case GL_UNSIGNED_SHORT:
			return sizeof(GLushort);
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", t);
			return 0;
	}
}

static void allocateUniformStorage(ActiveUniform *v)
{
	int size = shaderTypeSize(v->type);

	/* error check in getUniform */
	v->value = malloc(size*v->size);
}

/* TODO: G80 */
static int isIntType(GLenum type)
{
	switch (type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2_EXT:
		case GL_UNSIGNED_INT_VEC3_EXT:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 0;
		case GL_INT:
		case GL_BOOL:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
			return 1;
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", type);
			return 0;
	}
}

static int isUIntType(GLenum type)
{
	switch (type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_INT:
		case GL_BOOL:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
			return 0;
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2_EXT:
		case GL_UNSIGNED_INT_VEC3_EXT:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 1;
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", type);
			return 0;
	}
}

static int isMatrixType(GLenum type)
{
	switch (type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_INT:
		case GL_BOOL:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2_EXT:
		case GL_UNSIGNED_INT_VEC3_EXT:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 0;
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			return 1;
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", type);
			return 0;
	}
}

static int isSamplerType(GLenum type)
{
	switch (type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_INT:
		case GL_BOOL:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2_EXT:
		case GL_UNSIGNED_INT_VEC3_EXT:
		case GL_UNSIGNED_INT_VEC4_EXT:
			return 0;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_RECT_ARB:
		case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		case GL_SAMPLER_1D_ARRAY_EXT:
		case GL_SAMPLER_2D_ARRAY_EXT:
		case GL_SAMPLER_BUFFER_EXT:
		case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
		case GL_SAMPLER_CUBE_SHADOW_EXT:
		case GL_INT_SAMPLER_1D_EXT:
		case GL_INT_SAMPLER_2D_EXT:
		case GL_INT_SAMPLER_3D_EXT:
		case GL_INT_SAMPLER_CUBE_EXT:
		case GL_INT_SAMPLER_2D_RECT_EXT:
		case GL_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_INT_SAMPLER_BUFFER_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
			return 1;
		default:
			UT_NOTIFY_VA(LV_ERROR, "HMM, unkown shader variable type: %i", type);
			return 0;
	}
}

static int getUniform(GLuint progHandle, ActiveUniform *u)
{
	int error;

	allocateUniformStorage(u);
	if (!u->value) {
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}
	if (isIntType(u->type)) {
		ORIG_GL(glGetUniformiv)(progHandle, u->location, u->value);
	} else if (isUIntType(u->type)) {
		ORIG_GL(glGetUniformuivEXT)(progHandle, u->location, u->value);
	} else {
		ORIG_GL(glGetUniformfv)(progHandle, u->location, u->value);
	}
	error = glError();
	if (error) {
		free(u->value);
		return error;
	}
	return DBG_NO_ERROR;
}

static int setUniform(GLint progHandle, ActiveUniform *u)
{
	int error;

	UT_NOTIFY_VA(LV_INFO, "setUniform(%i, %s)\n",  progHandle, u->name);

	/* handle the special case of a uninitialized sampler uniform */
	if (isSamplerType(u->type)) {
		GLint numTexUnits, maxTexCoords, maxCombinedTextureImageUnits;
		ORIG_GL(glGetIntegerv)(GL_MAX_TEXTURE_COORDS, &maxTexCoords);
		ORIG_GL(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
				&maxCombinedTextureImageUnits);
		numTexUnits = maxTexCoords > maxCombinedTextureImageUnits ?
			maxTexCoords : maxCombinedTextureImageUnits;
		if (((GLint*)u->value)[0] < 0 || ((GLint*)u->value)[0] >= numTexUnits) {
			UT_NOTIFY_VA(LV_INFO,
					"Sampler \"%s\" seems to be uninitialized (%i/%i/%i); ignoring\n",
					u->name, ((GLint*)u->value)[0], maxTexCoords,
					maxCombinedTextureImageUnits);
			return DBG_NO_ERROR;
		}
	}

	GLint location = ORIG_GL(glGetUniformLocation)(progHandle, u->name);
	error = glError();
	if (error) {
		return error;
	}
	if (location < 0) {
		UT_NOTIFY_VA(LV_WARN, "A uniform named \"%s\" is not known to the program %s\n", u->name);
		return DBG_NO_ERROR;
	}
	if (isMatrixType(u->type)) {
		switch (u->type) {
			case GL_FLOAT_MAT2:
				ORIG_GL(glUniformMatrix2fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT2x3:
				ORIG_GL(glUniformMatrix2x3fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT2x4:
				ORIG_GL(glUniformMatrix2x4fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT3:
				ORIG_GL(glUniformMatrix3fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT3x2:
				ORIG_GL(glUniformMatrix3x2fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT3x4:
				ORIG_GL(glUniformMatrix3x4fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT4:
				ORIG_GL(glUniformMatrix4fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT4x2:
				ORIG_GL(glUniformMatrix4x2fv)(location, u->size, 0, u->value);
				break;
			case GL_FLOAT_MAT4x3:
				ORIG_GL(glUniformMatrix4x3fv)(location, u->size, 0, u->value);
				break;
		}
	} else if (isIntType(u->type)) {
		switch (uniformNumElements(u)) {
			case 1:
				ORIG_GL(glUniform1iv)(location, u->size, u->value);
				break;
			case 2:
				ORIG_GL(glUniform2iv)(location, u->size, u->value);
				break;
			case 3:
				ORIG_GL(glUniform3iv)(location, u->size, u->value);
				break;
			case 4:
				ORIG_GL(glUniform4iv)(location, u->size, u->value);
				break;
		}
	} else if (isUIntType(u->type)) {
		switch (uniformNumElements(u)) {
			case 1:
				ORIG_GL(glUniform1uivEXT)(location, u->size, u->value);
				break;
			case 2:
				ORIG_GL(glUniform2uivEXT)(location, u->size, u->value);
				break;
			case 3:
				ORIG_GL(glUniform3uivEXT)(location, u->size, u->value);
				break;
			case 4:
				ORIG_GL(glUniform4uivEXT)(location, u->size, u->value);
				break;
		}
	} else { /* float type */
		switch (uniformNumElements(u)) {
			case 1:
				ORIG_GL(glUniform1fv)(location, u->size, u->value);
				break;
			case 2:
				ORIG_GL(glUniform2fv)(location, u->size, u->value);
				break;
			case 3:
				ORIG_GL(glUniform3fv)(location, u->size, u->value);
				break;
			case 4:
				ORIG_GL(glUniform4fv)(location, u->size, u->value);
				break;
		}
	}

	error = glError();
	if (error) {
		return error;
	} else {
		return DBG_NO_ERROR;
	}
}

static int getActiveUniforms(ShaderProgram *shader)
{
	GLint maxLength;
	char *name = NULL;
	int i, error, numBaseUniforms, n;
	ActiveUniform *baseUniforms = NULL;

	ORIG_GL(glGetProgramiv)(shader->programHandle, GL_ACTIVE_UNIFORMS,
	                        &numBaseUniforms);
	ORIG_GL(glGetProgramiv)(shader->programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH,
		                        &maxLength);
	error = glError();
	if (error) {
		shader->numUniforms = 0;
		return error;
	}

	if (!(name = (char*)malloc(maxLength*sizeof(char)))) {
		UT_NOTIFY_VA(LV_ERROR, "Allocation failed: uniform name");
		shader->numUniforms = 0;
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	UT_NOTIFY_VA(LV_INFO, "ACTIVE UNIFORMS: %i", numBaseUniforms);

	if (!(baseUniforms = (ActiveUniform*)malloc(numBaseUniforms*sizeof(ActiveUniform)))) {
		UT_NOTIFY_VA(LV_ERROR, "Allocation failed: base uniforms");
		shader->numUniforms = 0;
		free(name);
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	shader->numUniforms = 0;

	for (i = 0; i < numBaseUniforms; i++) {
		ActiveUniform *u = &baseUniforms[i];
		ORIG_GL(glGetActiveUniform)(shader->programHandle, i, maxLength, NULL,
		                            &u->size, &u->type, name);
		error = glError();
		if (error) {
			free(name);
			free(baseUniforms);
			shader->numUniforms = 0;
			return error;
		}
		UT_NOTIFY_VA(LV_INFO, "FOUND UNIFORM: %s size=%i type=%s\n",
				name, u->size, lookupEnum(u->type));
		if (!strncmp(name, "gl_", 3)) {
			u->builtin = 1;
		} else {
			if (!(u->name = strdup(name))) {
				UT_NOTIFY_VA(LV_ERROR, "Allocation failed: uniform name");
				free(name);
				free(baseUniforms);
				shader->numUniforms = 0;
				return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
			}
			u->builtin = 0;
			shader->numUniforms += u->size;
		}
	}

	free(name);

	if (!(shader->uniforms = (ActiveUniform*)malloc(shader->numUniforms*sizeof(ActiveUniform)))) {
		UT_NOTIFY_VA(LV_ERROR, "Allocation failed: uniforms");
		shader->numUniforms = 0;
		free(baseUniforms);
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	n = 0;
	for (i = 0; i < numBaseUniforms; i++) {
		ActiveUniform *bu = &baseUniforms[i];
		if (!bu->builtin) {

			if (bu->size == 1) {
				ActiveUniform *u = &shader->uniforms[n];
				u->type = bu->type;
				u->size = 1;
				u->builtin = 0;
				u->value = NULL;
				if (!(u->name = strdup(bu->name))) {
					UT_NOTIFY_VA(LV_ERROR, "Allocation failed: uniform name\n");
					free(baseUniforms);
					shader->numUniforms = n;
					return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
				}
				u->location = ORIG_GL(glGetUniformLocation)(shader->programHandle, u->name);
				error = glError();
				if (error) {
					free(baseUniforms);
					free(u->name);
					shader->numUniforms = n;
					return error;
				}
				UT_NOTIFY_VA(LV_INFO, "SAVE UNIFORM: %s size=%i type=%s location=%i",
				         u->name, u->size, lookupEnum(u->type), u->location);
				error = getUniform(shader->programHandle, u);
				if (error) {
					free(u->name);
					free(baseUniforms);
					shader->numUniforms = n;
					return error;
				}
				n++;
			} else {
				int j;
				for (j = 0; j < bu->size; j++) {
					ActiveUniform *u = &shader->uniforms[n];
					u->type = bu->type;
					u->size = 1;
					u->builtin = 0;
					u->value = NULL;
					if (asprintf(&u->name, "%s[%i]", bu->name, j) < 0) {
						UT_NOTIFY_VA(LV_ERROR, "Allocation failed: uniform name");
						free(baseUniforms);
						shader->numUniforms = n;
						return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
					}
					u->location = ORIG_GL(glGetUniformLocation)(shader->programHandle,
								u->name);
					error = glError();
					if (error) {
						free(baseUniforms);
						free(u->name);
						shader->numUniforms = n;
						return error;
					}
					UT_NOTIFY_VA(LV_INFO, "SAVE UNIFORM: %s size=%i type=%s location=%i",
		    		        u->name, u->size, lookupEnum(u->type), u->location);
					error = getUniform(shader->programHandle, u);
					if (error) {
						free(u->name);
						free(baseUniforms);
						shader->numUniforms = n;
						return error;
					}
					n++;
				}
			}
		}
	}
	free(baseUniforms);
	return DBG_NO_ERROR;
}

static int getActiveAttributes(ShaderProgram *shader)
{
	GLint maxLength;
	char *name = NULL;
	int i, error;

	ORIG_GL(glGetProgramiv)(shader->programHandle, GL_ACTIVE_ATTRIBUTES,
	                        &shader->numAttributes);
	ORIG_GL(glGetProgramiv)(shader->programHandle,
	                        GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	error = glError();
	if (error) {
		shader->numAttributes = 0;
		return error;
	}

	if (!(name = (char*)malloc(maxLength*sizeof(char)))) {
		UT_NOTIFY_VA(LV_ERROR, "Allocation failed: attrib name");
		shader->numAttributes = 0;
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	UT_NOTIFY_VA(LV_INFO, "ACTIVE ATTRIBS: %i", shader->numAttributes);
	if (!(shader->attributes = (ActiveAttribute*)malloc(shader->numAttributes*
	                                              sizeof(ActiveAttribute)))) {
		UT_NOTIFY_VA(LV_ERROR, "Allocation failed: attributes");
		shader->numAttributes = 0;
		free(name);
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	for (i = 0; i < shader->numAttributes; i++) {
		ActiveAttribute *a = &shader->attributes[i];
		ORIG_GL(glGetActiveAttrib)(shader->programHandle, i, maxLength, NULL,
		                           &a->size, &a->type, name);
		error = glError();
		if (error) {
			shader->numAttributes = i;
			free(name);
			return error;
		}

		if (!(a->name = strdup(name))) {
			UT_NOTIFY_VA(DBGLVL_ERROR, "Allocation failed: attribute name");
			shader->numAttributes = i;
			free(name);
			return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
		}
		if (!strncmp(a->name, "gl_", 3)) {
			a->builtin = 1;
			a->location = -1;
		} else {
			a->builtin = 0;
			a->location = ORIG_GL(glGetAttribLocation)(shader->programHandle,
													   a->name);
			error = glError();
			if (error) {
				shader->numAttributes = i + 1;
				free(name);
				return error;
			}
		}
		UT_NOTIFY_VA(DBGLVL_INFO, "SAVED ATTRIB: %s size=%i type=%s, location=%i",
		         a->name, a->size, lookupEnum(a->type), a->location);
	}
	free(name);
	return DBG_NO_ERROR;
}

static int getProgramParameters(ShaderProgram *shader)
{
	ORIG_GL(glGetProgramiv)(shader->programHandle,
	                        GL_GEOMETRY_VERTICES_OUT_EXT,
	                        &shader->geoVerticesOut);
	ORIG_GL(glGetProgramiv)(shader->programHandle,
	                        GL_GEOMETRY_INPUT_TYPE_EXT,
	                        &shader->geoInputType);
	ORIG_GL(glGetProgramiv)(shader->programHandle,
	                        GL_GEOMETRY_OUTPUT_TYPE_EXT,
	                        &shader->geoOutputType);
	return glError();
}

int getShaderPrimitiveMode(void)
{
	switch(g.storedShader.geoOutputType) {
		case GL_POINTS:
			return GL_POINTS;
		case GL_LINES:
		case GL_LINE_STRIP:
		case GL_LINES_ADJACENCY_EXT:
		case GL_LINE_STRIP_ADJACENCY_EXT:
			return GL_LINES;
		case GL_TRIANGLES:
		case GL_TRIANGLE_STRIP:
		case GL_TRIANGLES_ADJACENCY_EXT:
		case GL_TRIANGLE_STRIP_ADJACENCY_EXT:
			return GL_TRIANGLES;
		default:
			/* FIXME error handling!! */
			return GL_POINTS;
	}
}

static int getShaderObjects(ShaderProgram *shader)
{
	GLuint *objects;
	int i, error;

	/* determine number of attached shader objects */
    ORIG_GL(glGetProgramiv)(shader->programHandle, GL_ATTACHED_SHADERS,
	                        &shader->numObjects);
	error = glError();
	if (error) {
		return error;
	}

    if (!(objects = (GLuint*) malloc(shader->numObjects * sizeof(GLuint)))) {
        UT_NOTIFY_VA(LV_ERROR, "not enough memory for temp. object handles");
        return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    if (!(shader->objects = (ShaderObject*) malloc(shader->numObjects *
	                                              sizeof(ShaderObject)))) {
        UT_NOTIFY_VA(LV_ERROR, "not enough memory for objects");
		free(objects);
        return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
    }

	/* get attached shader objects */
   	ORIG_GL(glGetAttachedShaders)(shader->programHandle, shader->numObjects, NULL,
	        objects);
	error = glError();
	if (error) {
		free(objects);
		return error;
	}

	/* for each shader object: get source and type */
    for (i = 0; i < shader->numObjects; i++) {

		shader->objects[i].handle = objects[i];

       	ORIG_GL(glGetShaderiv)(shader->objects[i].handle, GL_SHADER_SOURCE_LENGTH,
		        &shader->objects[i].srcLength);
       	ORIG_GL(glGetShaderiv)(shader->objects[i].handle, GL_SHADER_TYPE,
				&shader->objects[i].type);
		error = glError();
		if (error) {
			free(objects);
			return error;
		}

		if (!(shader->objects[i].src =
					(char*)malloc((shader->objects[i].srcLength + 1)*sizeof(char)))) {
			UT_NOTIFY_VA(LV_ERROR, "not enough memory for src string");
			free(objects);
			return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
		}

       	ORIG_GL(glGetShaderSource)(shader->objects[i].handle,
		                           shader->objects[i].srcLength,
		                           NULL, shader->objects[i].src);
		error = glError();
		if (error) {
			free(objects);
			return error;
		}
    }
	free(objects);
	return DBG_NO_ERROR;
}

static int getCurrentShader(ShaderProgram *shader)
{
//	int haveOpenGL_2_0_GLSL = checkGLVersionSupported(2, 0);
	int haveGeometryShader =  checkGLExtensionSupported("GL_EXT_geometry_shader4");
	int error;

	/* get handle of currently active GLSL shader program */
	ORIG_GL(glGetIntegerv)(GL_CURRENT_PROGRAM, (GLint*)&shader->programHandle);
	error = glError();
	if (error) {
		return error;
	}

	if (shader->programHandle == 0) {
		return 0;
	}

	/* get attached shader objects */
	error = getShaderObjects(shader);
	if (error) {
		return error;
	}

	/* get active uniforms and attributes */
	error = getActiveUniforms(shader);
	if (error) {
		return error;
	}
	error = getActiveAttributes(shader);
	if (error) {
		return error;
	}

	/* if geometry shader is supported, get program parameters */
	if (haveGeometryShader) {
		error = getProgramParameters(shader);
		if (error) {
			return error;
		}
	} else {
		shader->geoVerticesOut = 0;
		shader->geoOutputType = GL_NONE;
		shader->geoInputType = GL_NONE;
	}

	return DBG_NO_ERROR;
}

static void freeShaderProgram(ShaderProgram *shader)
{
	int i;

	for (i = 0; i < shader->numObjects; i++) {
		free(shader->objects[i].src);
	}
	free(shader->objects);
	shader->objects = NULL;
	shader->numObjects = 0;
	for (i = 0; i < shader->numUniforms; i++) {
		free(shader->uniforms[i].name);
		free(shader->uniforms[i].value);
	}
	free(shader->uniforms);
	shader->uniforms = NULL;
	shader->numUniforms = 0;
	for (i = 0; i < shader->numAttributes; i++) {
		free(shader->attributes[i].name);
	}
	free(shader->attributes);
	shader->attributes = NULL;
	shader->numAttributes = 0;
	shader->programHandle = 0;
}

static int getShaderResources(ShaderProgram *shader, struct TBuiltInResource *resources)
{
	ORIG_GL(glGetIntegerv)(GL_MAX_LIGHTS, &resources->maxLights);
	ORIG_GL(glGetIntegerv)(GL_MAX_CLIP_PLANES, &resources->maxClipPlanes);
	ORIG_GL(glGetIntegerv)(GL_MAX_TEXTURE_UNITS, &resources->maxTextureUnits);
	ORIG_GL(glGetIntegerv)(GL_MAX_TEXTURE_COORDS, &resources->maxTextureCoords);
	ORIG_GL(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &resources->maxVertexAttribs);
	ORIG_GL(glGetIntegerv)(GL_MAX_VERTEX_UNIFORM_COMPONENTS,
	                       &resources->maxVertexUniformComponents);
	ORIG_GL(glGetIntegerv)(GL_MAX_VARYING_FLOATS, &resources->maxVaryingFloats);
	ORIG_GL(glGetIntegerv)(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
	                       &resources->maxVertexTextureImageUnits);
	ORIG_GL(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	                       &resources->maxCombinedTextureImageUnits);
	ORIG_GL(glGetIntegerv)(GL_MAX_TEXTURE_IMAGE_UNITS,
	                       &resources->maxTextureImageUnits);
	ORIG_GL(glGetIntegerv)(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
	                       &resources->maxFragmentUniformComponents);
	ORIG_GL(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &resources->maxDrawBuffers);

	resources->framebufferObjectsSupported =
		checkGLExtensionSupported("GL_EXT_framebuffer_object");

	resources->geoShaderSupported =
	    checkGLExtensionSupported("GL_EXT_geometry_shader4");

	resources->transformFeedbackSupported = getTFBVersion() != TFBVersion_None;

	resources->geoVerticesOut = shader->geoVerticesOut;
	resources->geoInputType = shader->geoInputType;
	resources->geoOutputType = shader->geoOutputType;

	switch (shader->geoInputType) {
		case GL_POINTS:
			resources->geoVerticesIn = 1;
			break;
		case GL_LINES:
			resources->geoVerticesIn = 2;
			break;
		case GL_LINES_ADJACENCY_EXT:
			resources->geoVerticesIn = 4;
			break;
		case GL_TRIANGLES:
			resources->geoVerticesIn = 3;
			break;
		case GL_TRIANGLES_ADJACENCY_EXT:
			resources->geoVerticesIn = 6;
			break;
		default:
			return DBG_ERROR_INVALID_VALUE;
	}

	return glError();
}

static int serializeUniforms(
				const ShaderProgram *shader,
				char **serializedUniforms,
				GLint *serializedUniformsSize)
{
	int i;
	char *p;

	*serializedUniformsSize = 0;

	for (i = 0; i < shader->numUniforms; ++i) {
		ActiveUniform *u = &shader->uniforms[i];
		/* layout: nameLength|name|type|size|valueSize|value */
		*serializedUniformsSize += sizeof(GLint);
		*serializedUniformsSize += strlen(u->name);
		*serializedUniformsSize += sizeof(GLuint);
		*serializedUniformsSize += sizeof(GLint);
		*serializedUniformsSize += sizeof(GLint);
		*serializedUniformsSize += u->size*shaderTypeSize(u->type);
	}
	if (!(*serializedUniforms = (char*)malloc(*serializedUniformsSize)))
	{
		return 0;
	}

	p = *serializedUniforms;
	for (i = 0; i < shader->numUniforms; ++i) {
		ActiveUniform *u = &shader->uniforms[i];
		GLint nameLength = strlen(u->name);
		GLint valueSize = u->size*shaderTypeSize(u->type);
		/* layout: nameLength|name|type|size|valueSize|value */
		memcpy(p, &nameLength, sizeof(GLint));
		p += sizeof(GLint);
		memcpy(p, u->name, nameLength);
		p += nameLength;
		memcpy(p, &u->type, sizeof(GLuint));
		p += sizeof(GLuint);
		memcpy(p, &u->size, sizeof(GLint));
		p += sizeof(GLint);
		memcpy(p, &valueSize, sizeof(GLint));
		p += sizeof(GLint);
		memcpy(p, u->value, valueSize);
		p += valueSize;
	}
	return 1;
}

/*
	SHM IN:
		fname    : *
		operation: DBG_GET_SHADER_CODE
	SHM out:
		fname    : *
		result   : DBG_SHADER_CODE or DBG_ERROR_CODE on error
		numItems : number of returned shader codes (0 or 3)
		items[0] : pointer to vertex shader src
		items[1] : length of vertex shader src
		items[2] : pointer to geometry shader src
		items[3] : length of geometry shader src
		items[4] : pointer to fragment shader src
		items[5] : length of fragment shader src
		items[6] : pointer to shader resources (TBuiltInResource*)
		items[7] : number of active uniforms
		items[8] : size of serialized uniforms array
		items[9] : pointer to serialzied active uniforms
*/
void getShaderCode(void)
{
    char** source[3] = {NULL, NULL, NULL};
    char*  shaderSource[3] = {NULL, NULL, NULL};
	struct TBuiltInResource *shaderResources = NULL;
	GLint numUniforms = 0;
	char* serializedUniforms = NULL;
	GLint serializedUniformsSize = 0;
    int numSourceStrings[3] = {0, 0, 0};
    int lenSourceStrings[3] = {0, 0, 0};
	int error;

	ShaderProgram shader;
    DbgRec* rec;
    int i, j;

#ifdef _WIN32
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	rec = getThreadRecord(GetCurrentProcessId());
#else /* _WIN32 */
	rec = getThreadRecord(getpid());
#endif /* _WIN32 */

	/* clear smem */
	rec->numItems = 0;
	for (i = 0; i < 9; ++i) {
		rec->items[i] = 0;
	}

	error = getCurrentShader(&shader);
	if (error) {
		setErrorCode(error);
		return;
	}

	if (shader.programHandle == 0) {
		rec->result = DBG_SHADER_CODE;
    	rec->numItems = 0;
		return;
	}

    for (i = 0; i < shader.numObjects; i++) {
		void *tmpAlloc;
        int typeId = getTypeId(shader.objects[i].type);
        numSourceStrings[typeId]++;
        lenSourceStrings[typeId] += shader.objects[i].srcLength;
        tmpAlloc = realloc(source[typeId], numSourceStrings[typeId]*sizeof(char*));
        if (!tmpAlloc) {
            UT_NOTIFY_VA(LV_ERROR, "Allocating memory for shaders failed: %s\n",
					strerror(errno));
    		freeShaderProgram(&shader);
			for (i = 0; i < 3; i++) {
				free(source[i]);
			}
			setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
			return;
        }
        source[typeId] = tmpAlloc;
		source[typeId][numSourceStrings[typeId] - 1] = shader.objects[i].src;
        UT_NOTIFY_VA(LV_INFO, "source[%d][%d] = %s\n", typeId, numSourceStrings[typeId] - 1,
            shader.objects[i].src);
	}

    /* TODO: do something better than just append!!!!
     *   GLSL allows for multiple strings per objects and the order is important for reconstructing
     *   a correct single file code
     */

    for (i=0; i<3; i++) {
        if (numSourceStrings[i] > 0) {
            lenSourceStrings[i]++;
            if (!(shaderSource[i] = malloc(lenSourceStrings[i]*sizeof(char)))) {
                UT_NOTIFY_VA(LV_ERROR, "not enough memory to combine all your shaders\n");
				for (i = 0; i < 3; i++) {
					free(source[i]);
					free(shaderSource[i]);
				}
				freeShaderProgram(&shader);
               	setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
				return;
            }
            shaderSource[i][0] = '\0';
            for (j=0; j<numSourceStrings[i]; j++) {
                strcat(shaderSource[i], source[i][j]);
            }
            free(source[i]);
			source[i] = NULL;
        }
    }

	if (!serializeUniforms(&shader, &serializedUniforms, &serializedUniformsSize))
	{
		for (i = 0; i < 3; i++) {
			free(shaderSource[i]);
		}
		setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
		return;
	}
	numUniforms = shader.numUniforms;

    freeShaderProgram(&shader);

	/* query shader resources */
	if (!(shaderResources = malloc(sizeof(struct TBuiltInResource)))) {
		for (i = 0; i < 3; i++) {
			free(shaderSource[i]);
		}
		setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
		return;
	}
	error = getShaderResources(&shader, shaderResources);
	if (error) {
		for (i = 0; i < 3; i++) {
			free(shaderSource[i]);
		}
		free(serializedUniforms);
		free(shaderResources);
		setErrorCode(error);
		return;
	}

	/* set results */
	rec->result = DBG_SHADER_CODE;
    rec->numItems = 3;
    for (i=0; i<3; i++) {
        rec->items[2*i] = (ALIGNED_DATA) shaderSource[i];
        rec->items[2*i+1] = (ALIGNED_DATA) lenSourceStrings[i];
    }
	rec->items[6] = (ALIGNED_DATA)shaderResources;
	rec->items[7] = (ALIGNED_DATA)numUniforms;
	rec->items[8] = (ALIGNED_DATA)serializedUniformsSize;
	rec->items[9] = (ALIGNED_DATA)serializedUniforms;
}

/* TODO: error checking */
/*
	SHM IN:
		fname    : *
		operation: DBG_GET_SHADER_CODE
	SHM out:
		fname    : *
		result   : DBG_ERROR_CODE
*/
void storeActiveShader(void)
{
	setErrorCode(getCurrentShader(&g.storedShader));
}

/* TODO: error checking */
/*
	SHM IN:
		fname    : *
		operation: DBG_GET_SHADER_CODE
	SHM out:
		fname    : *
		result   : DBG_ERROR_CODE
*/
void restoreActiveShader(void)
{
	int error;

	ORIG_GL(glUseProgram)(g.storedShader.programHandle);
	error = glError();
	if (error) {
		setErrorCode(error);
		return;
	}
	freeShaderProgram(&g.storedShader);
	setErrorCode(DBG_NO_ERROR);
}

/* TODO: error checking */
static void printShaderInfoLog(GLhandleARB shader)
{
	int length;
	GLcharARB *log;

	ORIG_GL(glGetShaderiv)(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 1) {
		if (!(log = (GLcharARB*)malloc(length*sizeof(GLcharARB)))) {
			UT_NOTIFY_VA(LV_ERROR, "Allocation of mem for GLSL info log failed");
			exit(1);
		}
		ORIG_GL(glGetShaderInfoLog)(shader, length, NULL, log);
		UT_NOTIFY_VA(LV_INFO, "SHADER INFOLOG:\n%s", log);
		free(log);
	}
}

/* TODO: error checking */
static void printProgramInfoLog(GLhandleARB shader)
{
	int length;
	GLcharARB *log;

	ORIG_GL(glGetProgramiv)(shader, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		if (!(log = (GLcharARB*)malloc(length*sizeof(GLcharARB)))) {
			UT_NOTIFY_VA(LV_ERROR, "Allocation of mem for GLSL info log failed");
			exit(1);
		}
		ORIG_GL(glGetProgramInfoLog)(shader, length, NULL, log);
		UT_NOTIFY_VA(LV_INFO, "PROGRAM INFOLOG:\n%s", log);
		free(log);
	}
}

/* TODO: error checking */
static int attachShaderObject(GLint programHandle, GLenum type, const char *src)
{
	GLint shader, status;
	int error;

	UT_NOTIFY_VA(DBGLVL_COMPILERINFO,
	         "ATTACH SHADER: %s\n-----------------\n%s\n--------------\n",
	         lookupEnum(type), src);

	shader = ORIG_GL(glCreateShader)(type);
	error = glError();
	if (error) {
		return error;
	}
	ORIG_GL(glShaderSource)(shader, 1, (void*)&src, NULL);
	error = glError();
	if (error) {
		return error;
	}
	ORIG_GL(glCompileShader)(shader);
	 error = glError();
	if (error) {
		return error;
	}
	ORIG_GL(glGetShaderiv)(shader, GL_COMPILE_STATUS, &status);
	error = glError();
	if (error) {
		return error;
	}
	printShaderInfoLog(shader);
	error = glError();
	if (error) {
		return error;
	}
	if (!status) {
		UT_NOTIFY_VA(LV_ERROR, "DBG SHADER COMPILATION for %s failed!", lookupEnum(type));
		return DBG_ERROR_DBG_SHADER_COMPILE_FAILED;
	}
	ORIG_GL(glAttachShader)(programHandle, shader);
	error = glError();
	if (error) {
		return error;
	}
	ORIG_GL(glDeleteShader)(shader);
	error = glError();
	if (error) {
		return error;
	}
	return DBG_NO_ERROR;
}

/* TODO: error checking */
static void freeDbgShader(void)
{
	if (g.dbgShaderHandle != -1) {
		ORIG_GL(glDeleteProgram)(g.dbgShaderHandle);
		glError();
		g.dbgShaderHandle = -1;
	}
}

int loadDbgShader(const char* vshader, const char *gshader, const char *fshader,
                  int target, int forcePointPrimitiveMode)
{
	int haveGeometryShader =  checkGLExtensionSupported("GL_EXT_geometry_shader4");
	GLint status;
	int i, error;

	freeDbgShader();

	g.dbgShaderHandle = ORIG_GL(glCreateProgram)();
	error = glError();
	if (error) {
		return error;
	}

	UT_NOTIFY_VA(LV_INFO, "SET DBG SHADER: %p, %p, %p %i",
	         vshader, gshader, fshader, target);

	if (vshader) {
		error = attachShaderObject(g.dbgShaderHandle, GL_VERTEX_SHADER, vshader);
		if (error) {
			freeDbgShader();
			return error;
		}
	}
	if (gshader && target != DBG_TARGET_VERTEX_SHADER) {
		error = attachShaderObject(g.dbgShaderHandle, GL_GEOMETRY_SHADER_EXT,
		                           gshader);
		if (error) {
			freeDbgShader();
			return error;
		}
	}
	if (fshader) {
		error = attachShaderObject(g.dbgShaderHandle, GL_FRAGMENT_SHADER,
		                           fshader);
		if (error) {
			freeDbgShader();
			return error;
		}
	}

	/* copy execution environment of previous active shader */
	/* pre-link part */
	if (g.storedShader.programHandle == 0) {
		UT_NOTIFY_VA(LV_ERROR, "STORE CURRENTLY ACTIVE SHADER BEFORE SETTING DBG SHADER!");
		freeDbgShader();
		return DBG_ERROR_NO_STORED_SHADER;
	}

	for (i = 0; i < g.storedShader.numAttributes; i++) {
		ActiveAttribute *a = &g.storedShader.attributes[i];
		if (!a->builtin) {
			/* glGetAttribLocation is not allowed before link

			int location;
			if (haveOpenGL_2_0_GLSL) {
				location = ORIG_GL(glGetAttribLocation)(g.dbgShaderHandle, a->name);
				if (location != -1) {
					ORIG_GL(glBindAttribLocation)(g.dbgShaderHandle, a->location,
							a->name);
				}
			} else {
				location = ORIG_GL(glGetAttribLocationARB)(g.dbgShaderHandle, a->name);
				if (location != -1) {
					ORIG_GL(glBindAttribLocationARB)(g.dbgShaderHandle, a->location,
							a->name);
				}
			}
			*/
			ORIG_GL(glBindAttribLocation)(g.dbgShaderHandle, a->location,
					a->name);

			error = glError();
			if (error) {
				freeDbgShader();
				return error;
			}
			UT_NOTIFY_VA(LV_INFO, "BINDATTRIBLOCATION: %s -> %i", a->name, a->location);
		}
	}

	/* if geometry shader is supported, set program parameters */
	if (haveGeometryShader && gshader) {
		DMARK
		UT_NOTIFY_VA(LV_INFO, "SET PROGRAM PARAMETERS: "
				"GL_GEOMETRY_VERTICES_OUT_EXT=%i "
				"GL_GEOMETRY_INPUT_TYPE_EXT=%s "
				"GL_GEOMETRY_OUTPUT_TYPE_EXT=%s",
				g.storedShader.geoVerticesOut,
				lookupEnum(g.storedShader.geoInputType),
				lookupEnum(g.storedShader.geoOutputType));
		ORIG_GL(glProgramParameteriEXT)(g.dbgShaderHandle,
										GL_GEOMETRY_VERTICES_OUT_EXT,
										g.storedShader.geoVerticesOut);
		ORIG_GL(glProgramParameteriEXT)(g.dbgShaderHandle,
										GL_GEOMETRY_INPUT_TYPE_EXT,
										g.storedShader.geoInputType);
		if (forcePointPrimitiveMode) {
			ORIG_GL(glProgramParameteriEXT)(g.dbgShaderHandle,
											GL_GEOMETRY_OUTPUT_TYPE_EXT,
											GL_POINTS);
		} else {
			ORIG_GL(glProgramParameteriEXT)(g.dbgShaderHandle,
											GL_GEOMETRY_OUTPUT_TYPE_EXT,
											g.storedShader.geoOutputType);
		}
		error = glError();
		if (error) {
			freeDbgShader();
			return error;
		}
	}

	/* TODO: other state (point size, geometry shader, etc.) !!! */

	/* if debug target is vertex or geometry shader, force varyings active that
	 * are used in transform feedback
	*/
	if (target == DBG_TARGET_GEOMETRY_SHADER ||
	    target == DBG_TARGET_VERTEX_SHADER) {
		switch (getTFBVersion()) {
			case TFBVersion_NV:
				ORIG_GL(glActiveVaryingNV)(g.dbgShaderHandle, "dbgResult"/*TODO*/);
				error = glError();
				if (error) {
					freeDbgShader();
					return error;
				}
				break;
			case TFBVersion_EXT:
				{
					const char* dbgTFBVaryings[] = {"dbgResult"};
					ORIG_GL(glTransformFeedbackVaryingsEXT)(g.dbgShaderHandle, 1, dbgTFBVaryings, GL_SEPARATE_ATTRIBS_EXT);
					error = glError();
					if (error) {
						freeDbgShader();
						return error;
					}
				}
				break;
			default:
				UT_NOTIFY_VA(LV_ERROR, "Unknown TFB version!");
				return DBG_ERROR_INVALID_OPERATION;
		}
	}

	/* link debug shader */
	ORIG_GL(glLinkProgram)(g.dbgShaderHandle);
	ORIG_GL(glGetProgramiv)(g.dbgShaderHandle, GL_LINK_STATUS, &status);
	error = glError();
	if (error) {
		freeDbgShader();
		return error;
	}

	printProgramInfoLog(g.dbgShaderHandle);
	if (!status) {
		UT_NOTIFY_VA(LV_ERROR, "LINKING DBG SHADER FAILED!");
		freeDbgShader();
		return DBG_ERROR_DBG_SHADER_LINK_FAILED;
	}

	/* if debug target is vertex or geometry shader, specify varyings that
	 * are used in transform feedback
	*/
	if (target == DBG_TARGET_GEOMETRY_SHADER ||
	    target == DBG_TARGET_VERTEX_SHADER) {
		GLint location;

		switch (getTFBVersion()) {
			case TFBVersion_NV:
				{
					location = ORIG_GL(glGetVaryingLocationNV)(g.dbgShaderHandle, "dbgResult"/*TODO*/);
					if (location < 0) {
						UT_NOTIFY_VA(LV_ERROR, "dbgResult NOT ACTIVE VARYING");
						freeDbgShader();
						return DBG_ERROR_VARYING_INACTIVE;
					}
					ORIG_GL(glTransformFeedbackVaryingsNV)(g.dbgShaderHandle, 1, &location, GL_SEPARATE_ATTRIBS_NV);
					error = glError();
					if (error) {
						freeDbgShader();
						return error;
					}
				}
				break;
			case TFBVersion_EXT:
				/* nothing to be done, the EXT extension requires to specify the
				 * transform feedback varying outputs *befor* linking the shader
				 */
				break;
			default:
				return DBG_ERROR_INVALID_OPERATION;
		}
	}

	/* activate debug shader */
	ORIG_GL(glUseProgram)(g.dbgShaderHandle);
	error = glError();
	if (error) {
		freeDbgShader();
		return error;
	}

	/* copy execution environment of previous active shader */
	/* post-link part */
	for (i = 0; i < g.storedShader.numUniforms; i++) {
		ActiveUniform *u = &g.storedShader.uniforms[i];
		if (!u->builtin) {
			error = setUniform(g.dbgShaderHandle, u);
			if (error) {
				freeDbgShader();
				return error;
			}
		}
	}

	/* TODO: other state (GL_EXT_bindable_uniform etc.) !!! */

	return DBG_NO_ERROR;
}

/*
	SHM IN:
		fname    : *
		operation: DBG_SET_DBG_SHADER
		items[0] : pointer to vertex shader src
		items[1] : pointer to geometry shader src
		items[2] : pointer to fragment shader src
		items[3] : debug target
	SHM out:
		fname    : *
		result   : DBG_ERROR_CODE on error; else DBG_NO_ERROR
*/
void setDbgShader(void)
{
#ifdef _WIN32
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DbgRec *rec = getThreadRecord(GetCurrentProcessId());
#else /* _WIN32 */
	DbgRec *rec = getThreadRecord(getpid());
#endif /* _WIN32 */

	const char *vshader = (const char *)rec->items[0];
	const char *gshader = (const char *)rec->items[1];
	const char *fshader = (const char *)rec->items[2];
	int target = (int)rec->items[3];

	setErrorCode(loadDbgShader(vshader, gshader, fshader, target, 0));
}

