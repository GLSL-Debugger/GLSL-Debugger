/*
 * ShaderInput.h
 *
 *  Created on: 12.02.2014
 */

#ifndef TEST_SHADERINPUT_H_
#define TEST_SHADERINPUT_H_

#include "glslang/Interface/ShaderHolder.h"
#include <map>
#include <string>

typedef std::map<std::string, ShaderHolder*> ShadersList;

class ShaderInput {
public:
	static ShaderHolder* getShader(std::string name)
	{
		ShadersList::iterator shader = shaders.find(name);
		if (shader != shaders.end())
			return shader->second;
		ShaderHolder* holder = load(name);
		shaders[name] = holder;
		return holder;
	}
	static void setPath(std::string p)
	{
		path = p;
	}

private:
	static std::string path;
	static ShaderHolder* load(std::string);
	static ShadersList shaders;
};

#endif /* TEST_SHADERINPUT_H_ */
