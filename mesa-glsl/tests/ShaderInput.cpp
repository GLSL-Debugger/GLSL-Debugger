/*
 * ShaderInput.cpp
 *
 *  Created on: 12.02.2014
 */

#include "glsl/ralloc.h"
#include "misc.h"
#include "ShaderInput.h"
#include "glslang/Interface/Program.h"
#include "glslang/Interface/SymbolTable.h"
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
ResultsListMap ResultComparator::results;
ResultsFilesMap ResultComparator::results_dirs;
RulesMap ResultComparator::rules;
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
		shader->symbols = new(holder) sh_symbol_table;
		holder->shaders[shnum] = shader;
		shader->stage = shTypes[shnum];
		shader->source = source;
		shader->name = (char*) rzalloc_array(holder, char, fname.length());
		strcpy(shader->name, fname.c_str());

		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader_to_ast(holder->ctx, shader, 0);
		setlocale(LC_NUMERIC, old_locale);

		if (!shader->compile_status) {
			dbgPrint(DBGLVL_ERROR,
					"%s: Compilation failed, info log:\n%s\n", shader->name, shader->info_log);
			break;
		}
	}

	return holder;
}

static bool folderLoaded(ResultsFilesMap& dirs, std::string name)
{
	ResultsFilesMap::iterator fit = dirs.find(name);
	return fit != dirs.end();
}

static bool loadFile(ResultsList& r, const char* fname)
{
	std::ifstream file(fname);
	if (!file.is_open())
		return false;

	std::string line;
	while(std::getline(file, line))
		r.push_back(line);

	file.close();
	return true;
}

static void loadRules(RulesList& rules_map, std::string path)
{
	std::string filename = path + "/rules";
	char* source = test_load_text_file(NULL, filename.c_str());
	if (!source)
		return;
	dbgPrint(DBGLVL_INFO, "Found rules for %s.\n", path.c_str());
	std::istringstream ss(source);
	std::string line;
	while (std::getline(ss, line, '\n')) {
		if (line.empty() || line[0] == '#')
			continue;
		size_t delim = line.find(" ");
		CPPUNIT_ASSERT_MESSAGE("Bad rule found, rule: " + line,
					delim != std::string::npos);
		int id = std::stoi(line.substr(0, delim));
		rules_map[id].load(id, line.substr(delim + 1));
	}
}

void ResultComparator::loadResults(std::string name, std::string unit)
{
	for (int shnum = 0; shnum < SHADERS_PER_PROGRAM; ++shnum) {
		std::string shname = path + name + shExtensions[shnum];
		std::string fname = shname + "." + unit;
		if (getResults(fname) || folderLoaded(results_dirs, fname))
			continue;

		struct stat s;
		if (!stat(fname.c_str(), &s)) {
			if (s.st_mode & S_IFDIR) {
				int iter = 1;
				bool file_exists = true;
				std::vector<ResultsList>& dir = results_dirs[fname];
				// Open files starting from name 1 until file not found
				while (file_exists) {
					std::stringstream ss;
					ss << fname << "/" << iter++;
					ResultsList r;
					file_exists = loadFile(r, ss.str().c_str());
					if (file_exists)
						dir.push_back(r);
				}
				// Load rules for the dir
				loadRules(rules[fname], fname);
			} else if (s.st_mode & S_IFREG) {
				ResultsList& r = results[fname];
				if (!loadFile(r, fname.c_str()))
					dbgPrint(DBGLVL_INFO, "Test output %s not found. Skipping.\n", fname.c_str());
			} else {
				dbgPrint(DBGLVL_INFO, "%s is not regular file nor directory.\n", fname.c_str());
			}
		}
	}
}

void TestRule::load(int id, std::string str)
{
	line = id;
	size_t pos;
	if ((pos = str.find("save")) != std::string::npos) {
		type = tr_save;
	} else if ((pos = str.find("load")) != std::string::npos) {
		type = tr_load;
	}

	if ((pos = str.find("jump ")) != std::string::npos) {
		type = tr_jump;
		pos += 5;
		size_t sp;
		if ((sp = str.find(' ', pos)) != std::string::npos)
			jump_to = std::stoi(str.substr(pos, sp - pos));
	}

	if ((pos = str.find("bhvr ")) != std::string::npos) {
		type |= tr_bhvr;
		pos += 5;
		size_t sp = str.find(' ', pos);
		if (sp == std::string::npos)
			sp = str.length();
		std::string rule = str.substr(pos, sp - pos);
		if (!strcmp(rule.c_str(), "reset"))
			behaviour = DBG_BH_RESET;
		else if (!strcmp(rule.c_str(), "into"))
			behaviour = DBG_BH_JUMP_INTO;
		else if (!strcmp(rule.c_str(), "else"))
			behaviour = DBG_BH_FOLLOW_ELSE;
		else if (!strcmp(rule.c_str(), "over"))
			behaviour = DBG_BH_JUMP_OVER;
		else if (!strcmp(rule.c_str(), "next"))
			behaviour = DBG_BH_LOOP_NEXT_ITER;
		else if (!strcmp(rule.c_str(), "none"))
			behaviour = DBG_BH_NO_ACTION;

		if (!strcmp(rule.c_str(), "switch")) {
			pos += 7;
			sp = str.find(' ', pos);
			if (sp == std::string::npos)
				sp = str.length();
			if (pos < sp) {
				int branch = std::stoi(str.substr(pos, sp - pos));
				behaviour &= ~DBG_BH_SWITCH_BRANCH_LAST;
				behaviour += branch;
			}
		}
	}
}
