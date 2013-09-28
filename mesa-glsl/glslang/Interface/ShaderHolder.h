/*
 * ShaderHolder.h
 *
 *  Created on: 06.09.2013
 */

#ifndef SHADERHOLDER_H_
#define SHADERHOLDER_H_

//class TParseContext;


struct ShaderHolder {
	EShLanguage language;
	int debugOptions;
	struct gl_shader_program* program;
	//TParseContext* parse_context;

	// Initializator
	//TParseContext* getParseContext();
};


#endif /* SHADERHOLDER_H_ */
