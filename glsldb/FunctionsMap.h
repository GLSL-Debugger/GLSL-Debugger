#ifndef FUNCTIONSMAP_H
#define FUNCTIONSMAP_H

#include <unordered_map>
#include <string>

#include "debuglib.h"

typedef std::unordered_map<std::string, GLFunctionList*> GLFunctionsMap;

class FunctionsMap
{
public:
	static FunctionsMap& instance();
	void initialize();
	GLFunctionList* operator[](const std::string& name);
private:
	static FunctionsMap* _instance;
	GLFunctionsMap _map;


};

#endif
