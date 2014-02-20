/*
 * ShaderInput.cpp
 *
 *  Created on: 12.02.2014
 */

#include "glsl/ralloc.h"
#include "misc.h"
#include "ShaderInput.h"
#include "glslang/Interface/Program.h"
#include "glsldb/utils/dbgprint.h"
#include <locale.h>

#define SHADERS_PER_PROGRAM 3
static const std::string shExtensions[SHADERS_PER_PROGRAM] = {
	".vert",
	".geom",
	".frag" };

static gl_shader_stage shTypes[SHADERS_PER_PROGRAM] = {
	MESA_SHADER_VERTEX,
	MESA_SHADER_GEOMETRY,
	MESA_SHADER_FRAGMENT };

std::string ShaderInput::path = "./";
ShadersList ShaderInput::shaders;

ShaderHolder* ShaderInput::load(std::string name)
{
	struct gl_context *ctx = rzalloc(NULL, struct gl_context);
	test_initialize_context(ctx, API_OPENGL_COMPAT);

	ShaderHolder* holder = rzalloc(ctx, struct ShaderHolder);
	holder->ctx = ctx;

	for (int shnum = 0; shnum < SHADERS_PER_PROGRAM; ++shnum) {
		std::string fname = path + name + shExtensions[shnum];
		char* source = test_load_text_file(holder, fname.c_str());
		if (!source) {
			dbgPrint(DBGLVL_INFO, "Test shader %s not found. Skipping.\n", fname.c_str());
			continue;
		}

		holder->shaders = reralloc(holder, holder->shaders,
				struct AstShader*, holder->num_shaders + 1);
		AstShader* shader = rzalloc(holder, struct AstShader);
		holder->shaders[holder->num_shaders++] = shader;
		shader->stage = shTypes[shnum];
		shader->source = source;

		ShVariableList *vl = new ShVariableList;
		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader_to_ast(holder->ctx, shader, 0, vl);
		setlocale(LC_NUMERIC, old_locale);
		delete vl;

		if (!shader->compile_status) {
			dbgPrint(DBGLVL_ERROR,
					"%s: Compilation failed, info log:\n%s\n", fname.c_str(), shader->info_log);
			break;
		}
	}

	return holder;
}
