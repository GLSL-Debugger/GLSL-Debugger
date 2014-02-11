/*
 * locations.cpp
 *
 *  Created on: 05.01.2014
 */

#include "ShaderLang.h"
#include "glslang/Interface/Program.h"
#include "glslang/Interface/ShaderHolder.h"
#include "glsl/ralloc.h"
#include "glsldb/utils/dbgprint.h"
#include "misc.h"
#include <getopt.h>
#include <stdlib.h>
#include <locale.h>

extern int _mesa_glsl_debug;





int dump_ast = 0;
int dump_hir = 0;
int dump_lir = 0;
const struct option compiler_opts[] = {
	{ "version", required_argument, NULL, 'v' },
	{ NULL, 0, NULL, 0 }
};



int run_tests(AstShader* shader, ShVariableList* vl) {
	int count;
	// Traverse tree for scope and variable processing
	// Each node gets data holding list of variables changes with this
	// operation and about scope information
	count += ShaderVarTraverse(shader, vl);
	return count;
}


int main(int argc, char **argv)
{
	//_mesa_glsl_debug = 1;
	int status = EXIT_SUCCESS;
	struct gl_context local_ctx;
	struct gl_context *ctx = &local_ctx;
	bool glsl_es = false;
	int glsl_version = 150;

	setMaxDebugOutputLevel(DBGLVL_ALL);

	int c;
	int idx = 0;
	while ((c = getopt_long(argc, argv, "", compiler_opts, &idx)) != -1) {
		switch (c) {
		case 'v':
			glsl_version = strtol(optarg, NULL, 10);
			switch (glsl_version) {
			case 100:
			case 300:
				glsl_es = true;
				break;
			case 110:
			case 120:
			case 130:
			case 140:
			case 150:
			case 330:
				glsl_es = false;
				break;
			default:
				fprintf(stderr, "Unrecognized GLSL version `%s'\n", optarg);
				test_usage_fail(argv[0]);
				break;
			}
			break;
		default:
			break;
		}
	}

	if (argc <= optind)
		test_usage_fail(argv[0]);

	test_initialize_context(ctx, (glsl_es) ? API_OPENGLES2 : API_OPENGL_COMPAT);

	ShaderHolder* holder = rzalloc(ctx, struct ShaderHolder);
	holder->ctx = ctx;

	for (/* empty */; argc > optind; optind++) {
		ShVariableList *vl = new ShVariableList;
		holder->shaders = reralloc(holder, holder->shaders,
				struct AstShader*, holder->num_shaders + 1);
		AstShader* shader = rzalloc(holder, struct AstShader);
		holder->shaders[holder->num_shaders] = shader;
		holder->num_shaders++;

		const unsigned len = strlen(argv[optind]);
		if (len < 6)
			test_usage_fail(argv[0]);

		const char * const ext = &argv[optind][len - 5];
		if (strncmp(".vert", ext, 5) == 0 || strncmp(".glsl", ext, 5) == 0)
			shader->stage = MESA_SHADER_VERTEX;
		else if (strncmp(".geom", ext, 5) == 0)
			shader->stage = MESA_SHADER_GEOMETRY;
		else if (strncmp(".frag", ext, 5) == 0)
			shader->stage = MESA_SHADER_FRAGMENT;
		else
			test_usage_fail(argv[0]);



		shader->source = test_load_text_file(holder, argv[optind]);

		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader_to_ast(holder->ctx, shader, 0, vl);
		setlocale(LC_NUMERIC, old_locale);

		if (!shader->compile_status) {
			dbgPrint(DBGLVL_ERROR, "Compilation failed, info log:\n%s\n",
					 shader->info_log);
			break;
		}

		run_tests(shader, vl);
		delete vl;
	}

	ralloc_free(holder);
	return status;
}
