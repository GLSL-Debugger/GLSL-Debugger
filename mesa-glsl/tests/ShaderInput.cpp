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

static const std::string shExtensions[SHADERS_PER_PROGRAM] = {
	".vert",
	".geom",
	".frag" };

static gl_shader_stage shTypes[SHADERS_PER_PROGRAM] = {
	MESA_SHADER_VERTEX,
	MESA_SHADER_GEOMETRY,
	MESA_SHADER_FRAGMENT };

std::string Resource::path = "./";
ShadersList ShaderInput::shaders;
std::map<std::string, ResultsListMap> ResultComparator::results;


ShaderHolder* ShaderInput::load(std::string name)
{
	struct gl_context *ctx = rzalloc(NULL, struct gl_context);
	test_initialize_context(ctx, API_OPENGL_COMPAT);

	ShaderHolder* holder = rzalloc(ctx, struct ShaderHolder);
	holder->ctx = ctx;

	for (int shnum = 0; shnum < SHADERS_PER_PROGRAM; ++shnum) {
		std::string fname = path + name + shExtensions[shnum];
		char* source = test_load_text_file(holder, fname.c_str());
		holder->shaders = reralloc(holder, holder->shaders,
				struct AstShader*, holder->num_shaders + 1);
		holder->shaders[shnum] = NULL;
		holder->num_shaders++;

		if (!source) {
			dbgPrint(DBGLVL_INFO, "Test shader %s not found. Skipping.\n", fname.c_str());
			continue;
		}

		AstShader* shader = rzalloc(holder, struct AstShader);
		holder->shaders[shnum] = shader;
		shader->stage = shTypes[shnum];
		shader->source = source;
		shader->name = (char*) rzalloc_array(holder, char, fname.length());
		strcpy(shader->name, fname.c_str());

		ShVariableList *vl = new ShVariableList;
		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader_to_ast(holder->ctx, shader, 0, vl);
		setlocale(LC_NUMERIC, old_locale);
		delete vl;

		if (!shader->compile_status) {
			dbgPrint(DBGLVL_ERROR,
					"%s: Compilation failed, info log:\n%s\n", shader->name, shader->info_log);
			break;
		}
	}

	return holder;
}

void ResultComparator::loadResults(std::string name, std::string unit)
{
	for (int shnum = 0; shnum < SHADERS_PER_PROGRAM; ++shnum) {
		std::string shname = path + name + shExtensions[shnum];
		std::string fname = shname + "." + unit;
		if (getResults(shname, unit))
			continue;

		ResultsList& r = results[shname][unit];
		char* source = test_load_text_file(NULL, fname.c_str());
		if (!source) {
			dbgPrint(DBGLVL_INFO, "Test output %s not found. Skipping.\n", fname.c_str());
			continue;
		}

		char * pch;
		pch = strtok(source, "\n");
		while (pch != NULL) {
			r.push_back(std::string(pch));
			pch = strtok(NULL, "\n");
		}
	}

}
