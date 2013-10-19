/*
 * ShaderHolder.h
 *
 *  Created on: 06.09.2013
 */

#ifndef SHADERHOLDER_H_
#define SHADERHOLDER_H_

struct ShaderHolder {
	EShLanguage language;
	int debugOptions;
	struct gl_shader_program* program;
	struct gl_context* ctx;
};


#endif /* SHADERHOLDER_H_ */
