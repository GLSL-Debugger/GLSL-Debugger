/*
 * ShaderHolder.h
 *
 *  Created on: 06.09.2013
 */

#ifndef SHADERHOLDER_H_
#define SHADERHOLDER_H_

#include "mesa/main/mtypes.h"
struct exec_list;

struct AstShader {
	exec_list* head;
	unsigned version;
	const char* source;
	gl_shader_stage stage;
	bool compile_status;
	char* info_log;
	bool is_es;
};

struct ShaderHolder {
	int debug_options;
	EShLanguage language;
	AstShader** shaders;
	unsigned num_shaders;
	struct gl_context* ctx;
};


#endif /* SHADERHOLDER_H_ */
