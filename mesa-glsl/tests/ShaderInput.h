/*
 * ShaderInput.h
 *
 *  Created on: 12.02.2014
 */

#ifndef TEST_SHADERINPUT_H_
#define TEST_SHADERINPUT_H_

#include "glslang/Interface/ShaderHolder.h"
#include <map>
#include <vector>
#include <string>
#include <cppunit/TestAssert.h>

#define SHADERS_PER_PROGRAM 3

typedef std::map<std::string, ShaderHolder*> ShadersList;
typedef std::vector<std::string> ResultsList;
typedef std::map<std::string, ResultsList> ResultsListMap;

class Resource {
public:
	static void setPath(std::string p)
	{
		path = p;
	}

protected:
	static std::string path;
};

class ShaderInput: public Resource {
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

private:
	static ShaderHolder* load(std::string);
	static ShadersList shaders;
};

class ResultComparator: public Resource {
public:
	ResultComparator() :
			current(NULL), line(-1)
	{
	}

	void setCurrent(std::string name, std::string unit)
	{
		line = -1;
		current = getResults(name, unit);
	}

	static ResultsList* getResults(std::string name, std::string unit)
	{
		std::map<std::string, ResultsListMap>::iterator it = results.find(name);
		if (it != results.end()) {
			ResultsListMap::iterator rit = it->second.find(unit);
			if (rit != it->second.end())
				 return &rit->second;
		}
		return NULL;
	}

	void compareNext(std::string cmpline)
	{
		CPPUNIT_ASSERT(current);
		std::string orig_line = current->at(++line);
		std::stringstream ss;
		ss << "got line\n" << cmpline << "\nexcepted\n" << orig_line << "\nat line: " << line;
		CPPUNIT_ASSERT_MESSAGE(ss.str().c_str(), !orig_line.compare(cmpline));
	}

	static void loadResults(std::string name, std::string unit);

private:
	static std::map<std::string, ResultsListMap> results;
	ResultsList* current;
	int line;
};

#endif /* TEST_SHADERINPUT_H_ */
