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
#include <sys/stat.h>

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
std::map<std::string, ResultsFilesMap> ResultComparator::results_dirs;
std::vector<ResultsList> ResultComparator::dummy_files;


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

static bool folderLoaded(std::map<std::string, ResultsFilesMap>& dirs, std::string name, std::string unit)
{
	std::map<std::string, ResultsFilesMap>::iterator fit = dirs.find(name);
	if (fit != dirs.end()) {
		ResultsFilesMap::iterator rit = fit->second.find(unit);
		if (rit != fit->second.end())
			return true;
	}
	return false;
}

static bool loadFile(ResultsList& r, const char* fname)
{
	char* source = test_load_text_file(NULL, fname);
	if (!source)
		return false;

	char * pch;
	pch = strtok(source, "\n");
	while (pch != NULL) {
		r.push_back(std::string(pch));
		pch = strtok(NULL, "\n");
	}
	return true;
}

void ResultComparator::loadResults(std::string name, std::string unit)
{
	for (int shnum = 0; shnum < SHADERS_PER_PROGRAM; ++shnum) {
		std::string shname = path + name + shExtensions[shnum];
		std::string fname = shname + "." + unit;
		if (getResults(shname, unit) || folderLoaded(results_dirs, name, unit))
			continue;

		struct stat s;
		if (!stat(fname.c_str(), &s)) {
			if (s.st_mode & S_IFDIR) {
				int iter = 1;
				bool file_exists = true;
				std::stringstream ss;
				std::vector<ResultsList>& dir = results_dirs[shname][unit];
				// Open files starting from name 1 until file not found
				while (file_exists) {
					ss.clear();
					ss << fname << "/" << iter++;
					ResultsList r;
					file_exists = loadFile(r, ss.str().c_str());
					if (file_exists)
						dir.push_back(r);
				}
			} else if (s.st_mode & S_IFREG) {
				ResultsList& r = results[shname][unit];
				if (!loadFile(r, fname.c_str()))
					dbgPrint(DBGLVL_INFO, "Test output %s not found. Skipping.\n", fname.c_str());
			} else {
				dbgPrint(DBGLVL_INFO, "%s is not regular file nor directory.\n", fname.c_str());
			}
		}
	}

}
