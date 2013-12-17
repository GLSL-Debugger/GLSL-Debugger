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
#endif /* _WIN32 */
#include <string.h>

#include "debuglib.h"
#include "debuglibInternal.h"
#include "readback.h"
#include "glstate.h"
#include "shader.h"
#include "glenumerants.h"
#include "dbgprint.h"
#include "pfm.h"

#ifdef _WIN32
#include "trampolines.h"
#endif /* _WIN32 */

typedef struct {
	GLint name; /* 0 if not bound */
	GLint start; /* 0 if not specified, -1 if not bound */
	GLint size; /* 0 if not specified, -1 if not bound */
} TFBBufferBinding;

typedef struct {
	GLint attribute; /* 0 if no data */
	GLint components; /* 0 if no data */
	GLint index; /* 0 if no data or not indexed*/
} TFBRecord;

typedef struct {
	GLint mode;
	GLint attribs;
	GLint maxSeparateAttribs;
	TFBBufferBinding *buffer_bindings;
	TFBRecord *records; /* only when no program active */
} TFBState;

/* FIXME: not thread-safe! */
static struct {
	/* framebuffer dbg state */
	GLuint dbgFBO;
	GLuint dbgBufferFloat;
	GLuint dbgBufferInt;
	GLuint dbgDepthBuffer;
	/*GLuint dbgStencilBuffer;*/

	/* framebuffer saved state */
	GLint activeFBO;
	GLint activeDrawbuffer;
	GLint activeReadbuffer;
	GLboolean activeColorMask[4];
	GLboolean activeDepthMask;
	GLboolean activeAlphaTest;
	GLboolean activeStencilTest;
	GLboolean activeDepthTest;
	GLboolean activeBlending;
	GLint activeRedBits;
	GLint activeGreenBits;
	GLint activeBlueBits;
	GLint activeIndexBits;
	GLint activeAlphaBits;
	GLint activeDepthBits;
	GLint activeStencilBits;
	GLfloat *colorBuffer;
	GLfloat *depthBuffer;
	GLint *stencilBuffer;

	/* transform feedback dbg state */
	GLuint tfbBuffer;
	GLuint tfbQueries[2];
	TFBState savedTfbState;
} g;

typedef struct {
	GLboolean pack_swap_bytes;
	GLboolean pack_lsb_first;
	GLint pack_alignment;
	GLint pack_skip_pixels;
	GLint pack_skip_rows;
	GLboolean map_color;
	GLfloat red_scale;
	GLfloat green_scale;
	GLfloat blue_scale;
	GLfloat alpha_scale;
	GLfloat red_bias;
	GLfloat green_bias;
	GLfloat blue_bias;
	GLfloat alpha_bias;
	GLfloat post_convolution_red_scale;
	GLfloat post_convolution_green_scale;
	GLfloat post_convolution_blue_scale;
	GLfloat post_convolution_alpha_scale;
	GLfloat post_convolution_red_bias;
	GLfloat post_convolution_green_bias;
	GLfloat post_convolution_blue_bias;
	GLfloat post_convolution_alpha_bias;
	GLboolean color_table;
	GLboolean post_convolution_color_table;
	GLboolean post_color_matrix_color_table;
	GLdouble color_matrix[16];
	GLint matrix_mode;
	GLboolean convolution_1d;
	GLboolean convolution_2d;
	GLboolean separable_2d;
	GLfloat depth_bias;
	GLfloat depth_scale;
} pixelTransferState;

static void savePixelTransferState(pixelTransferState *savedState)
{
DMARK	/* pixel packing */
	ORIG_GL(glGetBooleanv)(GL_PACK_SWAP_BYTES, &savedState->pack_swap_bytes);
	ORIG_GL(glPixelStorei)(GL_PACK_SWAP_BYTES, GL_FALSE);
	ORIG_GL(glGetBooleanv)(GL_PACK_LSB_FIRST, &savedState->pack_lsb_first);
	ORIG_GL(glPixelStorei)(GL_PACK_LSB_FIRST, GL_FALSE);
	ORIG_GL(glGetIntegerv)(GL_PACK_ALIGNMENT, &savedState->pack_alignment);
	ORIG_GL(glPixelStorei)(GL_PACK_ALIGNMENT, 4);
	ORIG_GL(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &savedState->pack_skip_pixels);
	ORIG_GL(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
	ORIG_GL(glGetIntegerv)(GL_PACK_SKIP_ROWS, &savedState->pack_skip_rows);
	ORIG_GL(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);

	/* pixel transfer */
	ORIG_GL(glGetBooleanv)(GL_MAP_COLOR, &savedState->map_color);
	ORIG_GL(glPixelTransferi)(GL_MAP_COLOR, GL_FALSE);
	ORIG_GL(glGetFloatv)(GL_RED_SCALE, &savedState->red_scale);
	ORIG_GL(glPixelTransferf)(GL_RED_SCALE, 1.0);
	ORIG_GL(glGetFloatv)(GL_GREEN_SCALE, &savedState->green_scale);
	ORIG_GL(glPixelTransferf)(GL_GREEN_SCALE, 1.0);
	ORIG_GL(glGetFloatv)(GL_BLUE_SCALE, &savedState->blue_scale);
	ORIG_GL(glPixelTransferf)(GL_BLUE_SCALE, 1.0);
	ORIG_GL(glGetFloatv)(GL_ALPHA_SCALE, &savedState->alpha_scale);
	ORIG_GL(glPixelTransferf)(GL_ALPHA_SCALE, 1.0);
	ORIG_GL(glGetFloatv)(GL_RED_BIAS, &savedState->red_bias);
	ORIG_GL(glPixelTransferf)(GL_RED_BIAS, 0.0);
	ORIG_GL(glGetFloatv)(GL_GREEN_BIAS, &savedState->green_bias);
	ORIG_GL(glPixelTransferf)(GL_GREEN_BIAS, 0.0);
	ORIG_GL(glGetFloatv)(GL_BLUE_BIAS, &savedState->blue_bias);
	ORIG_GL(glPixelTransferf)(GL_BLUE_BIAS, 0.0);
	ORIG_GL(glGetFloatv)(GL_ALPHA_BIAS, &savedState->alpha_bias);
	ORIG_GL(glPixelTransferf)(GL_ALPHA_BIAS, 0.0);

	/* color table */
	if (checkGLExtensionSupported("GL_ARB_imaging") ||
			checkGLExtensionSupported("GL_EXT_color_table") ||
			checkGLExtensionSupported("GL_SGI_color_table")) {
		ORIG_GL(glGetBooleanv)(GL_COLOR_TABLE, &savedState->color_table);
		ORIG_GL(glDisable)(GL_COLOR_TABLE);
		ORIG_GL(glGetBooleanv)(GL_POST_CONVOLUTION_COLOR_TABLE, &savedState->post_convolution_color_table);
		ORIG_GL(glDisable)(GL_POST_CONVOLUTION_COLOR_TABLE);
		ORIG_GL(glGetBooleanv)(GL_POST_COLOR_MATRIX_COLOR_TABLE,
				&savedState->post_color_matrix_color_table);
		ORIG_GL(glDisable)(GL_POST_COLOR_MATRIX_COLOR_TABLE);
	}

	/* color matrix state */
	if (checkGLExtensionSupported("GL_SGI_color_matrix") ||
			checkGLExtensionSupported("GL_ARB_imaging")) {
		ORIG_GL(glGetDoublev)(GL_COLOR_MATRIX, savedState->color_matrix);
		ORIG_GL(glGetIntegerv)(GL_MATRIX_MODE, &savedState->matrix_mode);
		ORIG_GL(glMatrixMode)(GL_COLOR);
		ORIG_GL(glLoadIdentity)();
	}

	/* convolution */
	if (checkGLExtensionSupported("GL_ARB_imaging") ||
			checkGLExtensionSupported("GL_EXT_convolution")) {
		ORIG_GL(glGetBooleanv)(GL_CONVOLUTION_1D, &savedState->convolution_1d);
		ORIG_GL(glDisable)(GL_CONVOLUTION_1D);
		ORIG_GL(glGetBooleanv)(GL_CONVOLUTION_2D, &savedState->convolution_2d);
		ORIG_GL(glDisable)(GL_CONVOLUTION_2D);
		ORIG_GL(glGetBooleanv)(GL_SEPARABLE_2D, &savedState->separable_2d);
		ORIG_GL(glDisable)(GL_SEPARABLE_2D);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_RED_SCALE, &savedState->post_convolution_red_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_RED_SCALE, 1.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_GREEN_SCALE, &savedState->post_convolution_green_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_GREEN_SCALE, 1.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_BLUE_SCALE, &savedState->post_convolution_blue_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_BLUE_SCALE, 1.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_ALPHA_SCALE, &savedState->post_convolution_alpha_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_ALPHA_SCALE, 1.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_RED_BIAS, &savedState->post_convolution_red_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_RED_BIAS, 0.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_GREEN_BIAS, &savedState->post_convolution_green_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_GREEN_BIAS, 0.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_BLUE_BIAS, &savedState->post_convolution_blue_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_BLUE_BIAS, 0.0);
		ORIG_GL(glGetFloatv)(GL_POST_CONVOLUTION_ALPHA_BIAS, &savedState->post_convolution_alpha_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_ALPHA_BIAS, 0.0);
	}

	ORIG_GL(glGetFloatv)(GL_DEPTH_BIAS, &savedState->depth_bias);
	ORIG_GL(glPixelTransferf)(GL_DEPTH_BIAS, 0.0);
	ORIG_GL(glGetFloatv)(GL_DEPTH_SCALE, &savedState->depth_scale);
	ORIG_GL(glPixelTransferf)(GL_DEPTH_SCALE, 1.0);
}

static void restorePixelTransferState(pixelTransferState *savedState)
{
DMARK	/* pixel packing */
	ORIG_GL(glPixelStorei)(GL_PACK_SWAP_BYTES, savedState->pack_swap_bytes);
	ORIG_GL(glPixelStorei)(GL_PACK_LSB_FIRST, savedState->pack_lsb_first);
	ORIG_GL(glPixelStorei)(GL_PACK_ALIGNMENT, savedState->pack_alignment);
	ORIG_GL(glPixelStorei)(GL_PACK_SKIP_PIXELS, savedState->pack_skip_pixels);
	ORIG_GL(glPixelStorei)(GL_PACK_SKIP_ROWS, savedState->pack_skip_rows);

	/* pixel transfer */
	ORIG_GL(glPixelTransferi)(GL_MAP_COLOR, savedState->map_color);
	ORIG_GL(glPixelTransferf)(GL_RED_SCALE, savedState->red_scale);
	ORIG_GL(glPixelTransferf)(GL_GREEN_SCALE, savedState->green_scale);
	ORIG_GL(glPixelTransferf)(GL_BLUE_SCALE, savedState->blue_scale);
	ORIG_GL(glPixelTransferf)(GL_ALPHA_SCALE, savedState->alpha_scale);
	ORIG_GL(glPixelTransferf)(GL_RED_BIAS, savedState->red_bias);
	ORIG_GL(glPixelTransferf)(GL_GREEN_BIAS, savedState->green_bias);
	ORIG_GL(glPixelTransferf)(GL_BLUE_BIAS, savedState->blue_bias);
	ORIG_GL(glPixelTransferf)(GL_ALPHA_BIAS, savedState->alpha_bias);

	/* color table */
	if (checkGLExtensionSupported("GL_ARB_imaging") ||
			checkGLExtensionSupported("GL_EXT_color_table") ||
			checkGLExtensionSupported("GL_SGI_color_table")) {
		if (savedState->color_table) {
			ORIG_GL(glEnable)(GL_COLOR_TABLE);
		} else {
			ORIG_GL(glDisable)(GL_COLOR_TABLE);
		}
		if (savedState->post_convolution_color_table) {
			ORIG_GL(glEnable)(GL_POST_CONVOLUTION_COLOR_TABLE);
		} else {
			ORIG_GL(glDisable)(GL_POST_CONVOLUTION_COLOR_TABLE);
		}
		if (savedState->post_color_matrix_color_table) {
			ORIG_GL(glEnable)(GL_POST_COLOR_MATRIX_COLOR_TABLE);
		} else {
			ORIG_GL(glDisable)(GL_POST_COLOR_MATRIX_COLOR_TABLE);
		}
	}

	/* color matrix state */
	if (checkGLExtensionSupported("GL_SGI_color_matrix") ||
			checkGLExtensionSupported("GL_ARB_imaging")) {
		ORIG_GL(glLoadMatrixd)(savedState->color_matrix);
		ORIG_GL(glMatrixMode)(savedState->matrix_mode);
	}

	/* convolution */
	if (checkGLExtensionSupported("GL_ARB_imaging") ||
			checkGLExtensionSupported("GL_EXT_convolution")) {
		if (savedState->convolution_1d) {
			ORIG_GL(glEnable)(GL_CONVOLUTION_1D);
		} else {
			ORIG_GL(glDisable)(GL_CONVOLUTION_1D);
		}
		if (savedState->convolution_2d) {
			ORIG_GL(glEnable)(savedState->convolution_2d);
		} else {
			ORIG_GL(glDisable)(GL_CONVOLUTION_2D);
		}
		if (savedState->separable_2d) {
			ORIG_GL(glEnable)(GL_SEPARABLE_2D);
		} else {
			ORIG_GL(glDisable)(GL_SEPARABLE_2D);
		}
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_RED_SCALE, savedState->post_convolution_red_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_GREEN_SCALE, savedState->post_convolution_green_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_BLUE_SCALE,
				savedState->post_convolution_blue_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_ALPHA_SCALE,
				savedState->post_convolution_alpha_scale);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_RED_BIAS,
				savedState->post_convolution_red_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_GREEN_BIAS,
				savedState->post_convolution_green_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_BLUE_BIAS,
				savedState->post_convolution_blue_bias);
		ORIG_GL(glPixelTransferf)(GL_POST_CONVOLUTION_ALPHA_BIAS,
				savedState->post_convolution_alpha_bias);
	}

	ORIG_GL(glPixelTransferf)(GL_DEPTH_BIAS, savedState->depth_bias);
	ORIG_GL(glPixelTransferf)(GL_DEPTH_SCALE, savedState->depth_scale);
}

static int setDbgRenderState(int target, int alphaTestOption,
		int depthTestOption, int stencilTestOption, int blendingOption)
{
	DMARK
	if (target == DBG_TARGET_FRAGMENT_SHADER) {
		/* save state */
		ORIG_GL(glGetBooleanv)(GL_COLOR_WRITEMASK, g.activeColorMask);
		ORIG_GL(glGetBooleanv)(GL_DEPTH_WRITEMASK, &g.activeDepthMask);
		ORIG_GL(glGetBooleanv)(GL_ALPHA_TEST, &g.activeAlphaTest);
		ORIG_GL(glGetBooleanv)(GL_STENCIL_TEST, &g.activeStencilTest);
		ORIG_GL(glGetBooleanv)(GL_DEPTH_TEST, &g.activeDepthTest);
		ORIG_GL(glGetBooleanv)(GL_BLEND, &g.activeBlending);
		/* set state */ORIG_GL(glDrawBuffer)(GL_COLOR_ATTACHMENT0_EXT);
		ORIG_GL(glReadBuffer)(GL_COLOR_ATTACHMENT0_EXT);
		ORIG_GL(glColorMask)(GL_TRUE, GL_FALSE, GL_FALSE, g.activeColorMask[3]);
		dbgPrint(DBGLVL_INFO,
				"setDbgRenderState: stencilTestOption: %i "
				"alphaTestOption: %i depthTestOption: %i "
				"blendingOption: %i\n", stencilTestOption, alphaTestOption, depthTestOption, blendingOption);
		switch (stencilTestOption) {
		case DBG_PFT_FORCE_DISABLED:
			ORIG_GL(glDisable)(GL_STENCIL_TEST);
			break;
		case DBG_PFT_FORCE_ENABLED:
			ORIG_GL(glEnable)(GL_STENCIL_TEST);
			break;
		case DBG_PFT_KEEP:
		default:
			break;
		}
		switch (alphaTestOption) {
		case DBG_PFT_FORCE_DISABLED:
			ORIG_GL(glDisable)(GL_ALPHA_TEST);
			break;
		case DBG_PFT_FORCE_ENABLED:
			ORIG_GL(glEnable)(GL_ALPHA_TEST);
			break;
		case DBG_PFT_KEEP:
		default:
			break;
		}
		switch (depthTestOption) {
		case DBG_PFT_FORCE_DISABLED:
			ORIG_GL(glDisable)(GL_DEPTH_TEST);
			break;
		case DBG_PFT_FORCE_ENABLED:
			ORIG_GL(glEnable)(GL_DEPTH_TEST);
			break;
		case DBG_PFT_KEEP:
		default:
			break;
		}
		switch (blendingOption) {
		case DBG_PFT_FORCE_DISABLED:
			ORIG_GL(glDisable)(GL_BLEND);
			break;
		case DBG_PFT_FORCE_ENABLED:
			ORIG_GL(glEnable)(GL_BLEND);
			break;
		case DBG_PFT_KEEP:
		default:
			break;
		}
	} else {
	}
	return glError();
}

/* save transform feedback state */
static void saveTransformFeedbackState(TFBState *tfbState)
{
	dbgPrint(DBGLVL_INFO, "Saving TFB state:\n");

#if 0
	ORIG_GL(glGetIntegerv)(GL_TRANSFORM_FEEDBACK_BUFFER_MODE_NV,
			&tfbState->mode);
	dbgPrint(DBGLVL_INFO,"\tmode: %i\n", tfbState->mode);

	ORIG_GL(glGetIntegerv)(GL_TRANSFORM_FEEDBACK_ATTRIBS_NV, &tfbState->attribs);
	dbgPrint(DBGLVL_INFO,"\tattribs: %i\n", tfbState->attribs);

	ORIG_GL(glGetIntegerv)(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_NV,
			&tfbState->buffer_binding);
	dbgPrint(DBGLVL_INFO,"\tbuffer_binding: %i\n", tfbState->buffer_binding);

	ORIG_GL(glGetIntegerIndexedvEXT)(GL_TRANSFORM_FEEDBACK_RECORD_NV,XXXXXX);

	tfbState->buffer_bindings = malloc(tfbState->maxSeparateAttribs*sizeof(TFBBufferBinding));
	ORIG_GL(glGetIntegerIndexedvEXT)(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_NV,XXXXXX);

	ORIG_GL(glGetIntegerIndexedvEXT)(GL_TRANSFORM_FEEDBACK_BUFFER_START_NV,XXXXXX);

	ORIG_GL(glGetIntegerIndexedvEXT)(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE_NV,XXXXXX);
#else
	dbgPrint(DBGLVL_INFO, "\tTODO!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
}

static void restoreTransformFeedbackState(TFBState *tfbState)
{
	dbgPrint(DBGLVL_INFO, "Restoring TFB state:\n");
	dbgPrint(DBGLVL_INFO, "\tTODO!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static int restoreDbgRenderState(int target)
{
	DMARK
	if (target == DBG_TARGET_FRAGMENT_SHADER) {
		/* TODO: MRTs */
		ORIG_GL(glDrawBuffer)(g.activeDrawbuffer);
		ORIG_GL(glReadBuffer)(g.activeReadbuffer);
		ORIG_GL(glColorMask)(g.activeColorMask[0], g.activeColorMask[1],
				g.activeColorMask[2], g.activeColorMask[3]);
		ORIG_GL(glDepthMask)(g.activeDepthMask);
		if (g.activeStencilTest) {
			ORIG_GL(glEnable)(GL_STENCIL_TEST);
		} else {
			ORIG_GL(glDisable)(GL_STENCIL_TEST);
		}
		if (g.activeAlphaTest) {
			ORIG_GL(glEnable)(GL_ALPHA_TEST);
		} else {
			ORIG_GL(glDisable)(GL_ALPHA_TEST);
		}
		if (g.activeDepthTest) {
			ORIG_GL(glEnable)(GL_DEPTH_TEST);
		} else {
			ORIG_GL(glDisable)(GL_DEPTH_TEST);
		}
		if (g.activeBlending) {
			ORIG_GL(glEnable)(GL_BLEND);
		} else {
			ORIG_GL(glDisable)(GL_BLEND);
		}
	} else {
		/* restore transform feedback state */
		restoreTransformFeedbackState(&g.savedTfbState);
	}
	return glError();
}

static void setDbgOutputTargetVertexData(void)
{
	int error;

	/* save transform feedback state */
	saveTransformFeedbackState(&g.savedTfbState);
	error = glError();
	if (error) {
		setErrorCode(DBG_ERROR_INVALID_OPERATION);
		return;
	}

DMARK	/* setup queries for number of generated/written primitives */
	ORIG_GL(glGenQueries)(2, g.tfbQueries);
	if (setGLErrorCode()) {
		return;
	}

	/* setup vbos for transform feedback data */ORIG_GL(glGenBuffers)(1,
			&g.tfbBuffer);
	if (setGLErrorCode()) {
		ORIG_GL(glDeleteQueries)(2, g.tfbQueries);
		return;
	}
	ORIG_GL(glBindBuffer)(GL_ARRAY_BUFFER, g.tfbBuffer);
	ORIG_GL(glBufferData)(GL_ARRAY_BUFFER,
			TRANSFORM_FEEDBACK_BUFFER_SIZE * sizeof(GLfloat), NULL,
			GL_DYNAMIC_READ);
	if (setGLErrorCode()) {
		ORIG_GL(glDeleteBuffers)(1, &g.tfbBuffer);
		ORIG_GL(glDeleteQueries)(2, g.tfbQueries);
		return;
	}

	/* set base for transform feedback */
	switch (getTFBVersion()) {
	case TFBVersion_NV:
		ORIG_GL(glBindBufferBaseNV)(GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0,
				g.tfbBuffer);
		break;
	case TFBVersion_EXT:
		ORIG_GL(glBindBufferBaseEXT)(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0,
				g.tfbBuffer);
		break;
	default:
		dbgPrint(DBGLVL_ERROR, "Unhandled TFB version!\n");
		setErrorCode(DBG_ERROR_INVALID_OPERATION);
		return;
	}
	if (setGLErrorCode()) {
		ORIG_GL(glDeleteBuffers)(1, &g.tfbBuffer);
		ORIG_GL(glDeleteQueries)(2, g.tfbQueries);
		return;
	}

	error = saveGLState();
	if (error) {
		ORIG_GL(glDeleteBuffers)(1, &g.tfbBuffer);
		ORIG_GL(glDeleteQueries)(2, g.tfbQueries);
		setErrorCode(error);
		return;
	}
	setErrorCode(DBG_NO_ERROR);

}

int beginTransformFeedback(int primitiveType)
{
	int error;

DMARK
		dbgPrint(DBGLVL_INFO,
			"glBeginTransformFeedback expecting %s\n", lookupEnum(primitiveType));

	switch (getTFBVersion()) {
	case TFBVersion_NV:
		ORIG_GL(glBeginTransformFeedbackNV)(primitiveType);
		error = glError();
		if (error) {
			return error;
		}

		/* disable rasterization */
		ORIG_GL(glEnable)(GL_RASTERIZER_DISCARD_NV);

		/* start queries */
		ORIG_GL(glBeginQuery)(GL_PRIMITIVES_GENERATED_NV, g.tfbQueries[0]);
		ORIG_GL(glBeginQuery)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV,
				g.tfbQueries[1]);
		error = glError();
		break;
	case TFBVersion_EXT:
		ORIG_GL(glBeginTransformFeedbackEXT)(primitiveType);
		error = glError();
		if (error) {
			return error;
		}

		/* disable rasterization */
		ORIG_GL(glEnable)(GL_RASTERIZER_DISCARD_EXT);

		/* start queries */
		ORIG_GL(glBeginQuery)(GL_PRIMITIVES_GENERATED_EXT, g.tfbQueries[0]);
		ORIG_GL(glBeginQuery)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT,
				g.tfbQueries[1]);
		error = glError();
		break;
	default:
		dbgPrint(DBGLVL_ERROR, "Unhandled TFB version!\n");
		error = GL_INVALID_OPERATION;
	}

	return error;
}

int endTransformFeedback(int primitiveType, int numFloatsPerVertex,
		float **data, int *numPrimitives, int *numVertices)
{
	GLuint primitivesGenerated, primitivesWritten;
	void *mappedBuffer = NULL;
	int error;

	DMARK

	switch (getTFBVersion()) {
	case TFBVersion_NV:
		ORIG_GL(glEndQuery)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV);
		ORIG_GL(glEndQuery)(GL_PRIMITIVES_GENERATED_NV);
		error = glError();
		if (error) {
			return error;
		}

		ORIG_GL(glDisable)(GL_RASTERIZER_DISCARD_NV);
		ORIG_GL(glEndTransformFeedbackNV)();
		error = glError();
		if (error) {
			return error;
		}
		break;
	case TFBVersion_EXT:
		ORIG_GL(glEndQuery)(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT);
		ORIG_GL(glEndQuery)(GL_PRIMITIVES_GENERATED_EXT);
		error = glError();
		if (error) {
			return error;
		}

		ORIG_GL(glDisable)(GL_RASTERIZER_DISCARD_EXT);
		ORIG_GL(glEndTransformFeedbackEXT)();
		error = glError();
		if (error) {
			return error;
		}
		break;
	default:
		dbgPrint(DBGLVL_ERROR, "Unhandled TFB version!\n");
		return GL_INVALID_OPERATION;
	}

	/* read back number of primitives written */ORIG_GL(glGetQueryObjectuiv)(
			g.tfbQueries[0], GL_QUERY_RESULT, &primitivesGenerated);
	error = glError();
	if (error) {
		return error;
	}
	ORIG_GL(glGetQueryObjectuiv)(g.tfbQueries[1], GL_QUERY_RESULT,
			&primitivesWritten);
	error = glError();
	if (error) {
		return error;
	}
	dbgPrint(DBGLVL_INFO,
			"PRIMITIVES GENERATED/WRITTEN = %d/%d\n", primitivesGenerated, primitivesWritten);

	if (primitivesWritten != primitivesGenerated) {
		dbgPrint(DBGLVL_WARNING, "PRIMITIVES GENERATED > PRIMITIVES WRITTEN -> "
		"FEEDBACKBUFFER TOO SMALL!\n");
	}

	*numPrimitives = primitivesWritten;
	switch (primitiveType) {
	case GL_POINTS:
		*numVertices = primitivesWritten;
		break;
	case GL_LINES:
		*numVertices = 2 * primitivesWritten;
		break;
	case GL_TRIANGLES:
		*numVertices = 3 * primitivesWritten;
		break;
	}

	if (!(*data = malloc(*numVertices * numFloatsPerVertex * sizeof(GLfloat)))) {
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}
	mappedBuffer = ORIG_GL(glMapBuffer)(GL_ARRAY_BUFFER, GL_READ_ONLY);
	error = glError();
	if (error) {
		free(*data);
		*data = NULL;
		return error;
	}

	memcpy(*data, mappedBuffer,
			*numVertices * numFloatsPerVertex * sizeof(GLfloat));

	ORIG_GL(glUnmapBuffer)(GL_ARRAY_BUFFER);
	error = glError();
	if (error) {
		free(*data);
		*data = NULL;
		return error;
	}

	return DBG_NO_ERROR;
}

#ifdef DEBUG
static void writeDbgImage(const char *filename, int width, int height,
		int numComponents, float *data)
{
	PFMFile f = {data, width, height, numComponents, 1.0};
	pfmWrite(filename, &f);
}
#endif

static void setDbgOutputTargetFragmentData(int alphaTestOption,
		int depthTestOption, int stencilTestOption, int blendingOption)
{
	pixelTransferState savedState;
	GLint viewport[4];
	int error;

	DMARK
	g.colorBuffer = NULL;
	g.depthBuffer = NULL;
	g.stencilBuffer = NULL;

	ORIG_GL(glGetIntegerv)(GL_VIEWPORT, viewport);

	/* TODO: check for fbo support! Do it in debugger!*/

	/* check whether a fbo is active */
	ORIG_GL(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT, &g.activeFBO);

	/* store currently active draw buffer and bit depths */
	ORIG_GL(glGetIntegerv)(GL_DRAW_BUFFER, &g.activeDrawbuffer);
	/* TODO: MRT draw buffers */
	ORIG_GL(glGetIntegerv)(GL_RED_BITS,	&g.activeRedBits);
	ORIG_GL(glGetIntegerv)(GL_GREEN_BITS, &g.activeGreenBits);
	ORIG_GL(glGetIntegerv)(GL_BLUE_BITS, &g.activeBlueBits);
	ORIG_GL(glGetIntegerv)(GL_ALPHA_BITS, &g.activeAlphaBits);
	ORIG_GL(glGetIntegerv)(GL_INDEX_BITS, &g.activeIndexBits);
	ORIG_GL(glGetIntegerv)(GL_DEPTH_BITS, &g.activeDepthBits);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_BITS, &g.activeStencilBits);
	if (setGLErrorCode()) {
		return;
	}

	/* save pixel pipeline state */
	savePixelTransferState(&savedState);
	error = glError();
	if (error) {
		setErrorCode(DBG_ERROR_INVALID_OPERATION);
		return;
	}

	dbgPrint(DBGLVL_INFO,
			"ACTIVE BUFFER: %s r=%i g=%i b=%i a=%i i=%i d=%i s=%i\n", lookupEnum(g.activeDrawbuffer), g.activeRedBits, g.activeGreenBits, g.activeBlueBits, g.activeAlphaBits, g.activeIndexBits, g.activeDepthBits, g.activeStencilBits);

	/* store color buffer content */
	if (!(g.colorBuffer = (GLfloat*) malloc(
			4 * viewport[2] * viewport[3] * sizeof(GLfloat)))) {
		dbgPrint(DBGLVL_WARNING, "ALLOCATION OF COLOR BUFFER BACKUP FAILED\n");
		setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
		return;
	}
	ORIG_GL(glGetIntegerv)(GL_READ_BUFFER, &g.activeReadbuffer);
	ORIG_GL(glReadBuffer)(g.activeDrawbuffer);
	ORIG_GL(glReadPixels)(viewport[0], viewport[1], viewport[2], viewport[3],
			GL_RGBA, GL_FLOAT, g.colorBuffer);
	if (setGLErrorCode()) {
		return;
	}

	/* store depth buffer content */
	if (g.activeDepthBits) {
		if (!(g.depthBuffer = (GLfloat*) malloc(
				viewport[2] * viewport[3] * sizeof(GLfloat)))) {
			dbgPrint(DBGLVL_WARNING,
					"ALLOCATION OF DEPTH BUFFER BACKUP FAILED\n");
			free(g.colorBuffer);
			setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
			return;
		}
		ORIG_GL(glReadPixels)(viewport[0], viewport[1], viewport[2],
				viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, g.depthBuffer);
#if 0
		fprintf(stderr, "XXXXXXXXXXXXXX %f %f %f\n",
				g.depthBuffer[512*256-1], g.depthBuffer[512*256], g.depthBuffer[512*256+1]);
		writeDbgImage("DBG-ORIG-DEPTHBUFFER.pfm", viewport[2], viewport[3], 1, g.depthBuffer);
#endif
		if (setGLErrorCode()) {
			return;
		}
	}

	/* store stencil buffer content */
	if (g.activeStencilBits) {
		if (!(g.stencilBuffer = (GLint*) malloc(
				viewport[2] * viewport[3] * sizeof(GLint)))) {
			dbgPrint(DBGLVL_WARNING,
					"ALLOCATION OF STENCIL BUFFER BACKUP FAILED\n");
			free(g.colorBuffer);
			free(g.depthBuffer);
			setErrorCode(DBG_ERROR_MEMORY_ALLOCATION_FAILED);
			return;
		}
		ORIG_GL(glReadPixels)(viewport[0], viewport[1], viewport[2],
				viewport[3], GL_STENCIL_INDEX, GL_INT, g.stencilBuffer);
		if (setGLErrorCode()) {
			return;
		}
	}

	/* restore pixel pipeline state */
	restorePixelTransferState(&savedState);
	error = glError();
	if (error) {
		free(g.colorBuffer);
		free(g.depthBuffer);
		setErrorCode(DBG_ERROR_INVALID_OPERATION);
		return;
	}

	/* create a new fbo with a RGBA float attachment */ORIG_GL(glGenFramebuffersEXT)(
			1, &g.dbgFBO);
	ORIG_GL(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, g.dbgFBO);
	if (setGLErrorCode()) {
		return;
	}

	/* color attachment */ORIG_GL(glGenRenderbuffersEXT)(1, &g.dbgBufferFloat);
	ORIG_GL(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, g.dbgBufferFloat);
	ORIG_GL(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT, GL_RGBA32F_ARB,
			viewport[2], viewport[3]);
	ORIG_GL(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT,
			GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, g.dbgBufferFloat);
	if (setGLErrorCode()) {
		return;
	}

	/* stencil buffer attachment */
	if (g.activeStencilBits > 0) {
#if 0
		not working, yet :-(

				GLenum internalFormat;
				switch (g.activeStencilBits) {
					case 1: internalFormat = GL_STENCIL_INDEX1_EXT; break;
					case 4: internalFormat = GL_STENCIL_INDEX4_EXT; break;
					case 8: internalFormat = GL_STENCIL_INDEX8_EXT; break;
					case 16: internalFormat = GL_STENCIL_INDEX16_EXT; break;
					default:
					dbgPrint(DBGLVL_WARNING, "UNSUPPORTED STENCIL BUFFER BIT DEPTH: %i\n",
							g.activeStencilBits);
					setErrorCode(DBG_ERROR_INVALID_VALUE);
					return;
				}

				ORIG_GL(glGenRenderbuffersEXT)(1, &g.dbgStencilBuffer);
				ORIG_GL(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, g.dbgStencilBuffer);
				ORIG_GL(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT,
						internalFormat,
						viewport[2], viewport[3]);
				ORIG_GL(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT,
						GL_STENCIL_ATTACHMENT_EXT,
						GL_RENDERBUFFER_EXT,
						g.dbgStencilBuffer);
#endif
		ORIG_GL(glGenRenderbuffersEXT)(1, &g.dbgDepthBuffer);
		ORIG_GL(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, g.dbgDepthBuffer);
		ORIG_GL(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT,
				GL_DEPTH_STENCIL_NV, viewport[2], viewport[3]);
		ORIG_GL(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT,
				GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g.dbgDepthBuffer);
		ORIG_GL(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT,
				GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
				g.dbgDepthBuffer);
		if (setGLErrorCode()) {
			return;
		}
	} else {
		/* depth buffer attachment */
		if (g.activeDepthBits > 0) {
			ORIG_GL(glGenRenderbuffersEXT)(1, &g.dbgDepthBuffer);
			ORIG_GL(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT,
					g.dbgDepthBuffer);
			ORIG_GL(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT,
					GL_DEPTH_COMPONENT24, viewport[2], viewport[3]);
			ORIG_GL(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT,
					GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
					g.dbgDepthBuffer);
			if (setGLErrorCode()) {
				return;
			}
		}
	}

	/* check framebuffer completeness */
	error = ORIG_GL(glCheckFramebufferStatusEXT)(GL_FRAMEBUFFER_EXT);
	switch (error) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	default:
		setErrorCode(error);
		return;
	}

	/* disable everything that could interfere when writting the debug result to
	 * the debug buffer and setup draw and read buffer.
	 */
	error = setDbgRenderState(DBG_TARGET_FRAGMENT_SHADER, alphaTestOption,
			depthTestOption, stencilTestOption, blendingOption);
	if (error) {
		setErrorCode(error);
		return;
	}

	error = saveGLState();
	if (error) {
		setErrorCode(error);
		return;
	}

	setErrorCode(DBG_NO_ERROR);
}

void setDbgOutputTarget(void)
{
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);

	DMARK
	switch (rec->items[0]) {
	case DBG_TARGET_VERTEX_SHADER:
	case DBG_TARGET_GEOMETRY_SHADER:
		setDbgOutputTargetVertexData();
		break;
	case DBG_TARGET_FRAGMENT_SHADER:
		setDbgOutputTargetFragmentData((int) rec->items[1], (int) rec->items[2],
				(int) rec->items[3], (int) rec->items[4]);
		break;
	default:
		setErrorCode(DBG_ERROR_INVALID_DBG_TARGET);
		return;
	}
}

static void restoreOutputTargetFragmentData(void)
{
	int error;

	DMARK

	free(g.colorBuffer);
	g.colorBuffer = NULL;

	ORIG_GL(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, g.activeFBO);
	ORIG_GL(glDeleteRenderbuffersEXT)(1, &g.dbgBufferFloat);
	if (setGLErrorCode()) {
		return;
	}
	if (g.activeDepthBits > 0 || g.activeStencilBits > 0) {
		free(g.depthBuffer);
		g.depthBuffer = NULL;
		ORIG_GL(glDeleteRenderbuffersEXT)(1, &g.dbgDepthBuffer);
	}
	if (g.activeStencilBits > 0) {
		free(g.stencilBuffer);
		g.stencilBuffer = NULL;
		/*ORIG_GL(glDeleteRenderbuffersEXT)(1, &g.dbgStencilBuffer);*/
	}
	ORIG_GL(glDeleteFramebuffersEXT)(1, &g.dbgFBO);
	if (!setGLErrorCode()) {
		setErrorCode(DBG_NO_ERROR);
	}

	error = restoreDbgRenderState(DBG_TARGET_FRAGMENT_SHADER);
	if (error) {
		setErrorCode(error);
		return;
	}

}

static void restoreOutputTargetVertexData(void)
{
	int error;

DMARK	ORIG_GL(glDeleteBuffers)(1, &g.tfbBuffer);
	ORIG_GL(glDeleteQueries)(2, g.tfbQueries);
	if (!setGLErrorCode()) {
		setErrorCode(DBG_NO_ERROR);
	}

	error = restoreDbgRenderState(DBG_TARGET_VERTEX_SHADER);
	if (error) {
		setErrorCode(error);
		return;
	}
}

void restoreOutputTarget(void)
{
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);
	int error;

	DMARK

	error = restoreGLState();
	if (error) {
		setErrorCode(error);
		return;
	}

	switch (rec->items[0]) {
	case DBG_TARGET_VERTEX_SHADER:
	case DBG_TARGET_GEOMETRY_SHADER:
		restoreOutputTargetVertexData();
		break;
	case DBG_TARGET_FRAGMENT_SHADER:
		restoreOutputTargetFragmentData();
		break;
	default:
		setErrorCode(DBG_ERROR_INVALID_DBG_TARGET);
		return;
	}

}

int readBackRenderBuffer(int numComponents, int dataFormat, int *width,
		int *height, void **buffer)
{
	pixelTransferState savedState;
	GLint viewport[4];
	int format, lineWidth;
	void *line;
	char *bf, *bb;
	int j, error;
	int formatSize;

DMARK	ORIG_GL(glGetIntegerv)(GL_VIEWPORT, viewport);

	switch (numComponents) {
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		dbgPrint(DBGLVL_WARNING, "readBackRenderBuffer "
		"Error: requested %i components\n", numComponents);
		return DBG_ERROR_READBACK_INVALID_COMPONENTS;
	}
	switch (dataFormat) {
	case GL_FLOAT:
		formatSize = sizeof(GLfloat);
		break;
	case GL_INT:
		formatSize = sizeof(GLint);
		break;
	case GL_UNSIGNED_INT:
		formatSize = sizeof(GLuint);
		break;
	default:
		dbgPrint(DBGLVL_WARNING, "readBackRenderBuffer "
		"Error: requested format %i invalid\n", format);
		return DBG_ERROR_READBACK_INVALID_FORMAT;
	}

	if (!(*buffer = malloc(
			numComponents * viewport[2] * viewport[3] * formatSize)) || !(line =
			malloc(numComponents * viewport[2] * formatSize))) {
		dbgPrint(DBGLVL_WARNING,
				"readBackRenderBuffer: Allocation of %i bytes failed\n", numComponents*viewport[2]*viewport[3]*formatSize);
		return DBG_ERROR_MEMORY_ALLOCATION_FAILED;
	}

	error = glError();
	if (error) {
		free(*buffer);
		return error;
	}
	savePixelTransferState(&savedState);
	error = glError();
	if (error) {
		free(*buffer);
		return error;
	}
	ORIG_GL(glReadPixels)(viewport[0], viewport[1], viewport[2], viewport[3],
			format, dataFormat, *buffer);
	error = glError();
	if (error) {
		free(*buffer);
		return error;
	}
	restorePixelTransferState(&savedState);
	error = glError();
	if (error) {
		free(*buffer);
		return error;
	}

	*width = viewport[2];
	*height = viewport[3];

	/* flip buffer content */
	lineWidth = numComponents * viewport[2] * sizeof(float);
	bf = (char*) *buffer;
	bb = (char*) *buffer + (viewport[3] - 1) * lineWidth;
	for (j = 0; j < viewport[3] / 2; j++) {
		memcpy(line, bf, lineWidth);
		memcpy(bf, bb, lineWidth);
		memcpy(bb, line, lineWidth);
		bf += lineWidth;
		bb -= lineWidth;
	}
	free(line);

	return DBG_NO_ERROR;
}

/*
 SHM IN:
 fname    : *
 operation: DBG_READ_RENDER_BUFFER
 items[0] : number of components to read (1:R, 3:RGB, 4:RGBA)
 SHM out:
 fname    : *
 result: DBG_READBACK_RESULT_FRAGMENT_DATA or DBG_ERROR_CODE on error
 items[0] : buffer address
 items[1] : image width
 items[2] : image height
 */
void readRenderBuffer(void)
{
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);
	int numComponents = (int) rec->items[0];
	int width, height, error;
	void *buffer;

	DMARK
	error = readBackRenderBuffer(numComponents, GL_FLOAT, &width, &height,
			&buffer);
	if (error != DBG_NO_ERROR) {
		setErrorCode(error);
	} else {
		rec->result = DBG_READBACK_RESULT_FRAGMENT_DATA;
		rec->items[0] = (ALIGNED_DATA) buffer;
		rec->items[1] = (ALIGNED_DATA) width;
		rec->items[2] = (ALIGNED_DATA) height;
	}
}

typedef struct {
	GLboolean color_write_mask[4];
	GLboolean depth_write_mask;
	GLboolean index_write_mask;
	GLboolean stencil_write_mask;

	GLboolean alpha_test;
	GLint alpha_test_func;
	GLfloat alpha_test_ref;

	GLboolean depth_test;
	GLint depth_func;

	GLboolean scissor_test;
	GLint scissor_box[4];

	GLboolean stencil_test;
	GLint stencil_fail;
	GLint stencil_func;
	GLint stencil_pass_depth_fail;
	GLint stencil_pass_depth_pass;
	GLint stencil_ref;
	GLint stencil_value_mask;

	GLboolean blend;
	GLint blend_dst;
	GLint blend_equation;
	GLint blend_src;

	GLboolean fog;

	GLboolean texture_1D;
	GLboolean texture_2D;
	GLboolean texture_3D;

	GLint shader_handle;
} pixelCopyState;

static void saveCopyState(pixelCopyState *savedState)
{
	/* Masks */
	ORIG_GL(glGetBooleanv)(GL_COLOR_WRITEMASK, savedState->color_write_mask);
	ORIG_GL(glGetBooleanv)(GL_DEPTH_WRITEMASK, &savedState->depth_write_mask);
	ORIG_GL(glGetBooleanv)(GL_INDEX_WRITEMASK, &savedState->index_write_mask);
	ORIG_GL(glGetBooleanv)(GL_STENCIL_WRITEMASK,
			&savedState->stencil_write_mask);

	/* Tests */ORIG_GL(glGetBooleanv)(GL_ALPHA_TEST, &savedState->alpha_test);
	ORIG_GL(glGetIntegerv)(GL_ALPHA_TEST_FUNC, &savedState->alpha_test_func);
	ORIG_GL(glGetFloatv)(GL_ALPHA_TEST_REF, &savedState->alpha_test_ref);

	ORIG_GL(glGetBooleanv)(GL_DEPTH_TEST, &savedState->depth_test);
	ORIG_GL(glGetIntegerv)(GL_DEPTH_FUNC, &savedState->depth_func);

	ORIG_GL(glGetBooleanv)(GL_SCISSOR_TEST, &savedState->scissor_test);
	ORIG_GL(glGetIntegerv)(GL_SCISSOR_BOX, savedState->scissor_box);

	ORIG_GL(glGetBooleanv)(GL_STENCIL_TEST, &savedState->stencil_test);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_FAIL, &savedState->stencil_fail);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_FUNC, &savedState->stencil_func);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_PASS_DEPTH_FAIL,
			&savedState->stencil_pass_depth_fail);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_PASS_DEPTH_PASS,
			&savedState->stencil_pass_depth_pass);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_REF, &savedState->stencil_ref);
	ORIG_GL(glGetIntegerv)(GL_STENCIL_VALUE_MASK,
			&savedState->stencil_value_mask);

	/* Blending */ORIG_GL(glGetBooleanv)(GL_BLEND, &savedState->blend);
	ORIG_GL(glGetIntegerv)(GL_BLEND_DST, &savedState->blend_dst);
	ORIG_GL(glGetIntegerv)(GL_BLEND_EQUATION, &savedState->blend_equation);
	ORIG_GL(glGetIntegerv)(GL_BLEND_SRC, &savedState->blend_src);

	/* Fog */ORIG_GL(glGetBooleanv)(GL_FOG, &savedState->fog);

	/* Texture */ORIG_GL(glGetBooleanv)(GL_TEXTURE_1D, &savedState->texture_1D);
	ORIG_GL(glGetBooleanv)(GL_TEXTURE_2D, &savedState->texture_2D);
	ORIG_GL(glGetBooleanv)(GL_TEXTURE_3D, &savedState->texture_3D);

	/* Fragment Program */
	ORIG_GL(glGetIntegerv)(GL_CURRENT_PROGRAM, &savedState->shader_handle);
}

static void restoreCopyState(pixelCopyState *savedState)
{
	/* Masks */
	ORIG_GL(glColorMask)(savedState->color_write_mask[0],
			savedState->color_write_mask[1], savedState->color_write_mask[2],
			savedState->color_write_mask[3]);
	ORIG_GL(glDepthMask)(savedState->depth_write_mask);
	ORIG_GL(glIndexMask)(savedState->index_write_mask);
	ORIG_GL(glStencilMask)(savedState->stencil_write_mask);

	/* Tests */
	if (savedState->alpha_test == GL_TRUE) {
		ORIG_GL(glEnable)(GL_ALPHA_TEST);
	} else {
		ORIG_GL(glDisable)(GL_ALPHA_TEST);
	}
	ORIG_GL(glAlphaFunc)(savedState->alpha_test_func,
			savedState->alpha_test_ref);

	if (savedState->depth_test == GL_TRUE) {
		ORIG_GL(glEnable)(GL_DEPTH_TEST);
	} else {
		ORIG_GL(glDisable)(GL_DEPTH_TEST);
	}
	ORIG_GL(glDepthFunc)(savedState->depth_func);

	if (savedState->scissor_test == GL_TRUE) {
		ORIG_GL(glEnable)(GL_SCISSOR_TEST);
	} else {
		ORIG_GL(glDisable)(GL_SCISSOR_TEST);
	}
	ORIG_GL(glScissor)(savedState->scissor_box[0], savedState->scissor_box[1],
			savedState->scissor_box[2], savedState->scissor_box[3]);

	if (savedState->stencil_test == GL_TRUE) {
		ORIG_GL(glEnable)(GL_STENCIL_TEST);
	} else {
		ORIG_GL(glDisable)(GL_STENCIL_TEST);
	}

	ORIG_GL(glStencilFunc)(savedState->stencil_func, savedState->stencil_ref,
			savedState->stencil_value_mask);

	ORIG_GL(glStencilOp)(savedState->stencil_fail,
			savedState->stencil_pass_depth_fail,
			savedState->stencil_pass_depth_pass);

	/* Blending */
	if (savedState->blend == GL_TRUE) {
		ORIG_GL(glEnable)(GL_BLEND);
	} else {
		ORIG_GL(glDisable)(GL_BLEND);
	}
	ORIG_GL(glBlendFunc)(savedState->blend_src, savedState->blend_dst);
	ORIG_GL(glBlendEquation)(savedState->blend_equation);

	/* Fog */
	if (savedState->fog == GL_TRUE) {
		ORIG_GL(glEnable)(GL_FOG);
	} else {
		ORIG_GL(glDisable)(GL_FOG);
	}

	/* Texture */
	if (savedState->texture_1D) {
		ORIG_GL(glEnable)(GL_TEXTURE_1D);
	} else {
		ORIG_GL(glDisable)(GL_TEXTURE_1D);
	}
	if (savedState->texture_2D) {
		ORIG_GL(glEnable)(GL_TEXTURE_2D);
	} else {
		ORIG_GL(glDisable)(GL_TEXTURE_2D);
	}
	if (savedState->texture_3D) {
		ORIG_GL(glEnable)(GL_TEXTURE_3D);
	} else {
		ORIG_GL(glDisable)(GL_TEXTURE_3D);
	}

	/* Fragment Program */
	ORIG_GL(glUseProgram)(savedState->shader_handle);
}

typedef enum {
	CS_COLOR,
	CS_DEPTH,
	CS_INDEX,
	CS_STENCIL
} copyStateTarget;

static void setCopyState(copyStateTarget csTarget)
{
	/* Masks */
	if (csTarget == CS_COLOR) {
		ORIG_GL(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	} else {
		ORIG_GL(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	if (csTarget == CS_DEPTH) {
		ORIG_GL(glDepthMask)(GL_TRUE);
	} else {
		ORIG_GL(glDepthMask)(GL_FALSE);
	}
	if (csTarget == CS_INDEX) {
		ORIG_GL(glIndexMask)(GL_TRUE);
	} else {
		ORIG_GL(glIndexMask)(GL_FALSE);
	}
	if (csTarget == CS_STENCIL) {
		ORIG_GL(glStencilMask)(GL_TRUE);
	} else {
		ORIG_GL(glStencilMask)(GL_FALSE);
	}

	/* Tests */ORIG_GL(glDisable)(GL_ALPHA_TEST);
	if (csTarget == CS_DEPTH) {
		ORIG_GL(glEnable)(GL_DEPTH_TEST);
		ORIG_GL(glDepthFunc)(GL_ALWAYS);
	} else {
		ORIG_GL(glDisable)(GL_DEPTH_TEST);
	}
	ORIG_GL(glDisable)(GL_SCISSOR_TEST);
	ORIG_GL(glDisable)(GL_STENCIL_TEST);

	/* Blending */ORIG_GL(glDisable)(GL_BLEND);

	/* Fog */ORIG_GL(glDisable)(GL_FOG);

	/* Texture */ORIG_GL(glDisable)(GL_TEXTURE_1D);
	ORIG_GL(glDisable)(GL_TEXTURE_2D);
	ORIG_GL(glDisable)(GL_TEXTURE_3D);

	/* Fragment Program */
	ORIG_GL(glUseProgram)(0);
}

/*
 SHM IN:
 fname    : *
 operation: DBG_CLEAR_RENDER_BUFFER
 items[0] : bitwise OR of several values indicating which buffer is to be
 cleared or copied, allowed values are DBG_CLEAR_RGB,
 DBG_CLEAR_ALPHA, DBG_CLEAR_DEPTH, and DBG_CLEAR_STENCIL
 items[1] : clear value red (float!)
 items[2] : clear value green (float!)
 items[3] : clear value blue (float!)
 items[4] : clear value alpha (float!)
 items[5] : clear value depth (float!)
 items[6] : clear value stencil
 SHM out:
 fname    : *
 result: DBG_ERROR_CODE on error
 */
void clearRenderBuffer(void)
{
#ifndef _WIN32
	pid_t pid = getpid();
#else /* _WIN32 */
	/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
	DWORD pid = GetCurrentProcessId();
#endif /* _WIN32 */
	DbgRec *rec = getThreadRecord(pid);
	GLbitfield clearBits = 0;

	GLfloat clearColor[4];
	GLfloat clearDepth;
	GLint clearStencil;
	GLint viewport[4];
	GLfloat rasterPos[4];
	GLfloat projectionMatrix[16];
	GLfloat modelViewMatrix[16];
	GLint matrixMode;

	pixelCopyState copyState;

	DMARK

	/* save state */
	saveCopyState(&copyState);

	ORIG_GL(glGetIntegerv)(GL_VIEWPORT, viewport);
	ORIG_GL(glGetFloatv)(GL_CURRENT_RASTER_POSITION, rasterPos);
	ORIG_GL(glGetFloatv)(GL_PROJECTION_MATRIX, projectionMatrix);
	ORIG_GL(glGetFloatv)(GL_MODELVIEW_MATRIX, modelViewMatrix);
	ORIG_GL(glGetIntegerv)(GL_MATRIX_MODE, &matrixMode);

	ORIG_GL(glMatrixMode)(GL_PROJECTION);
	ORIG_GL(glLoadIdentity)();
	ORIG_GL(glMatrixMode)(GL_MODELVIEW);
	ORIG_GL(glLoadIdentity)();
	ORIG_GL(glRasterPos2i)(viewport[0], viewport[1]);
	ORIG_GL(glWindowPos2i)(viewport[0], viewport[1]);

	dbgPrint(DBGLVL_INFO,
			"clearRenderBuffer: clearRGB: %li (%f %f %f) "
			"clearAlpha: %li (%f) "
			"clearDepth: %li (%f) clearStencil: %li (%i)\n", rec->items[0] & DBG_CLEAR_RGB, *(float*)&rec->items[1], *(float*)&rec->items[2], *(float*)&rec->items[3], rec->items[0] & DBG_CLEAR_ALPHA, *(float*)&rec->items[4], rec->items[0] & DBG_CLEAR_DEPTH, *(float*)&rec->items[5], rec->items[0] & DBG_CLEAR_STENCIL, (GLint)rec->items[6]);

	/* check gl error */
	if (setGLErrorCode()) {
		DMARK
		return;
	}

	/* depth buffer */
	if (g.activeDepthBits > 0) {
		if (rec->items[0] & DBG_CLEAR_DEPTH) {
			clearBits |= GL_DEPTH_BUFFER_BIT;
			ORIG_GL(glGetFloatv)(GL_DEPTH_CLEAR_VALUE, &clearDepth);
			ORIG_GL(glClearDepth)(*(float*) &rec->items[5]);
		} else {
			/* copy depth buffer content */
			setCopyState(CS_DEPTH);
			ORIG_GL(glDrawPixels)(viewport[2], viewport[3], GL_DEPTH_COMPONENT,
					GL_FLOAT, g.depthBuffer);
#ifdef DEBUG
			{
				float *data = (float*)malloc(viewport[2]*viewport[3]*sizeof(GLfloat));
				ORIG_GL(glReadPixels)(viewport[0], viewport[1], viewport[2], viewport[3],
						GL_DEPTH_COMPONENT, GL_FLOAT, data);
				fprintf(stderr, "XXXXXXXXXXXXXX %f %f %f\n",
						g.depthBuffer[512*256-1], g.depthBuffer[512*256], g.depthBuffer[512*256+1]);
				writeDbgImage("DBG-STORED-DEPTHBUFFER.pfm", viewport[2], viewport[3], 1, g.depthBuffer);
				fprintf(stderr, "XXXXXXXXXXXXXX %f %f %f\n", data[512*256-1], data[512*256], data[512*256+1]);
				writeDbgImage("DBG-FBO-DEPTHBUFFER.pfm", viewport[2], viewport[3], 1, data);
				free(data);
			}
#endif
		}
	}

	/* stencil buffer */
	if (g.activeStencilBits > 0) {
		if (rec->items[0] & DBG_CLEAR_STENCIL) {
			clearBits |= GL_STENCIL_BUFFER_BIT;
			ORIG_GL(glGetIntegerv)(GL_STENCIL_CLEAR_VALUE, &clearStencil);
			ORIG_GL(glClearStencil)((GLint) rec->items[6]);
		} else {
			/* copy stencil buffer content */
			setCopyState(CS_STENCIL);
			ORIG_GL(glDrawPixels)(viewport[2], viewport[3], GL_STENCIL_INDEX,
					GL_INT, g.stencilBuffer);
		}
	}

	/* color buffer */
	if (rec->items[0] & DBG_CLEAR_RGB) {
		clearBits |= GL_COLOR_BUFFER_BIT;
	}
	if (g.activeAlphaBits && (rec->items[0] & DBG_CLEAR_ALPHA)) {
		clearBits |= GL_COLOR_BUFFER_BIT;
	}
	if ((rec->items[0] & DBG_CLEAR_RGB) || (rec->items[0] & DBG_CLEAR_ALPHA)) {
		ORIG_GL(glGetFloatv)(GL_COLOR_CLEAR_VALUE, clearColor);
		ORIG_GL(glClearColor)(*(float*) &rec->items[1],
				*(float*) &rec->items[2], *(float*) &rec->items[3],
				*(float*) &rec->items[4]);
		ORIG_GL(glColorMask)(rec->items[0] & DBG_CLEAR_RGB,
				rec->items[0] & DBG_CLEAR_RGB, rec->items[0] & DBG_CLEAR_RGB,
				g.activeAlphaBits && (rec->items[0] & DBG_CLEAR_ALPHA));
	}
	if (rec->items[0] & DBG_CLEAR_DEPTH) {
		ORIG_GL(glDepthMask)(GL_TRUE);
	} else {
		ORIG_GL(glDepthMask)(GL_FALSE);
	}
	if (rec->items[0] & DBG_CLEAR_STENCIL) {
		ORIG_GL(glStencilMask)(GL_TRUE);
	} else {
		ORIG_GL(glStencilMask)(GL_FALSE);
	}
	dbgPrint(DBGLVL_INFO, "glClear: %s\n", dissectBitfield(clearBits));
	ORIG_GL(glClear)(clearBits);

	/* copy color buffer content */
	if (!(rec->items[0] & DBG_CLEAR_RGB)
			|| !(rec->items[0] & DBG_CLEAR_ALPHA)) {
		setCopyState(CS_COLOR);
		ORIG_GL(glColorMask)(!(rec->items[0] & DBG_CLEAR_RGB),
				!(rec->items[0] & DBG_CLEAR_RGB),
				!(rec->items[0] & DBG_CLEAR_RGB),
				!(rec->items[0] & DBG_CLEAR_ALPHA));
		ORIG_GL(glDrawPixels)(viewport[2], viewport[3], GL_RGBA, GL_FLOAT,
				g.colorBuffer);
	}

	/* restore clear values and masks */
	if ((rec->items[0] & DBG_CLEAR_RGB) || (rec->items[0] & DBG_CLEAR_ALPHA)) {
		ORIG_GL(glClearColor)(clearColor[0], clearColor[1], clearColor[2],
				clearColor[3]);
		ORIG_GL(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	if (g.activeDepthBits > 0 && (rec->items[0] & DBG_CLEAR_DEPTH)) {
		ORIG_GL(glClearDepth)(clearDepth);
	}
	if (g.activeStencilBits > 0 && (rec->items[0] & DBG_CLEAR_STENCIL)) {
		ORIG_GL(glClearStencil)(clearStencil);
	}

	/* restore state */
	restoreCopyState(&copyState);
	ORIG_GL(glRasterPos4fv)(rasterPos);
	ORIG_GL(glMatrixMode)(GL_PROJECTION);
	ORIG_GL(glLoadMatrixf)(projectionMatrix);
	ORIG_GL(glMatrixMode)(GL_MODELVIEW);
	ORIG_GL(glLoadMatrixf)(modelViewMatrix);
	ORIG_GL(glMatrixMode)(matrixMode);

	/* check gl error */
	if (setGLErrorCode()) {
		return;
	}
	setErrorCode(DBG_NO_ERROR);
}

