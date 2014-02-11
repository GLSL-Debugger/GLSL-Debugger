/*
 * misc.cpp
 *
 *  Created on: 11.02.2014
 */
#include <stdio.h>
#include "glsl/ralloc.h"
#include "mesa/main/mtypes.h"
#include "glsl/standalone_scaffolding.h"


void test_initialize_context(struct gl_context *ctx, gl_api api)
{
	initialize_context_to_defaults(ctx, api);

	/* The standalone compiler needs to claim support for almost
	 * everything in order to compile the built-in functions.
	 */
	ctx->Const.GLSLVersion = 150;
	ctx->Extensions.ARB_ES3_compatibility = true;

	switch (ctx->Const.GLSLVersion) {
	case 100:
		ctx->Const.MaxClipPlanes = 0;
		ctx->Const.MaxCombinedTextureImageUnits = 8;
		ctx->Const.MaxDrawBuffers = 2;
		ctx->Const.MinProgramTexelOffset = 0;
		ctx->Const.MaxProgramTexelOffset = 0;
		ctx->Const.MaxLights = 0;
		ctx->Const.MaxTextureCoordUnits = 0;
		ctx->Const.MaxTextureUnits = 8;

		ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = 8;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 0;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = 128 * 4;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxInputComponents = 0; /* not used */
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents = 32;

		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits =
				ctx->Const.MaxCombinedTextureImageUnits;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = 16 * 4;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents =
				ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents = 0; /* not used */

		ctx->Const.MaxVarying = ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents
				/ 4;
		break;
	case 110:
	case 120:
		ctx->Const.MaxClipPlanes = 6;
		ctx->Const.MaxCombinedTextureImageUnits = 2;
		ctx->Const.MaxDrawBuffers = 1;
		ctx->Const.MinProgramTexelOffset = 0;
		ctx->Const.MaxProgramTexelOffset = 0;
		ctx->Const.MaxLights = 8;
		ctx->Const.MaxTextureCoordUnits = 2;
		ctx->Const.MaxTextureUnits = 2;

		ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 0;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = 512;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxInputComponents = 0; /* not used */
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents = 32;

		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits =
				ctx->Const.MaxCombinedTextureImageUnits;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = 64;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents =
				ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents = 0; /* not used */

		ctx->Const.MaxVarying = ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents
				/ 4;
		break;
	case 130:
	case 140:
		ctx->Const.MaxClipPlanes = 8;
		ctx->Const.MaxCombinedTextureImageUnits = 16;
		ctx->Const.MaxDrawBuffers = 8;
		ctx->Const.MinProgramTexelOffset = -8;
		ctx->Const.MaxProgramTexelOffset = 7;
		ctx->Const.MaxLights = 8;
		ctx->Const.MaxTextureCoordUnits = 8;
		ctx->Const.MaxTextureUnits = 2;

		ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxInputComponents = 0; /* not used */
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents = 64;

		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents =
				ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents = 0; /* not used */

		ctx->Const.MaxVarying = ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents
				/ 4;
		break;
	case 150:
	case 330:
		ctx->Const.MaxClipPlanes = 8;
		ctx->Const.MaxDrawBuffers = 8;
		ctx->Const.MinProgramTexelOffset = -8;
		ctx->Const.MaxProgramTexelOffset = 7;
		ctx->Const.MaxLights = 8;
		ctx->Const.MaxTextureCoordUnits = 8;
		ctx->Const.MaxTextureUnits = 2;

		ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxInputComponents = 0; /* not used */
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents = 64;

		ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxInputComponents =
				ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents;
		ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxOutputComponents = 128;

		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents =
				ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxOutputComponents;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents = 0; /* not used */

		ctx->Const.MaxCombinedTextureImageUnits =
				ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits
						+ ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxTextureImageUnits
						+ ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits;

		ctx->Const.MaxGeometryOutputVertices = 256;
		ctx->Const.MaxGeometryTotalOutputComponents = 1024;

//      ctx->Const.MaxGeometryVaryingComponents = 64;

		ctx->Const.MaxVarying = 60 / 4;
		break;
	case 300:
		ctx->Const.MaxClipPlanes = 8;
		ctx->Const.MaxCombinedTextureImageUnits = 32;
		ctx->Const.MaxDrawBuffers = 4;
		ctx->Const.MinProgramTexelOffset = -8;
		ctx->Const.MaxProgramTexelOffset = 7;
		ctx->Const.MaxLights = 0;
		ctx->Const.MaxTextureCoordUnits = 0;
		ctx->Const.MaxTextureUnits = 0;

		ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = 1024;
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxInputComponents = 0; /* not used */
		ctx->Const.Program[MESA_SHADER_VERTEX].MaxOutputComponents = 16 * 4;

		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits = 16;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = 224;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents = 15 * 4;
		ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxOutputComponents = 0; /* not used */

		ctx->Const.MaxVarying = ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxInputComponents
				/ 4;
		break;
	}

	ctx->Driver.NewShader = _mesa_new_shader;
}

void test_usage_fail(const char *name)
{

	const char *header = "%s - run tests for certain shader\n"
			"usage: %s <file.vert | file.geom | file.frag>\n";
	printf(header, name, name);
	exit(EXIT_FAILURE);
}

char* test_load_text_file(void *ctx, const char *file_name)
{
	char *text = NULL;
	size_t size;
	size_t total_read = 0;
	FILE *fp = fopen(file_name, "rb");

	if (!fp) {
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	text = (char *) ralloc_size(ctx, size + 1);
	if (text != NULL) {
		do {
			size_t bytes = fread(text + total_read, 1, size - total_read, fp);
			if (bytes < size - total_read) {
				free(text);
				text = NULL;
				break;
			}

			if (bytes == 0) {
				break;
			}

			total_read += bytes;
		} while (total_read < size);

		text[total_read] = '\0';
	}

	fclose(fp);

	return text;
}
