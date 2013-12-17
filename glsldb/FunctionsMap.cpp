#include "FunctionsMap.h"
#include "notify.h"
FunctionsMap* FunctionsMap::_instance = 0;
extern "C" GLFunctionList glFunctions[];

FunctionsMap& FunctionsMap::instance()
{
	if (!_instance) {
		_instance = new FunctionsMap;
	}
	return *_instance;
}

void FunctionsMap::initialize()
{
	uint32_t i = 0;
	while (glFunctions[i].fname) {
		_map[glFunctions[i].fname] = &glFunctions[i];
		++i;
	}
	UT_NOTIFY(LV_INFO, _map.size() << " registered GL-Functions");
}
GLFunctionList* FunctionsMap::operator[](const std::string& name)
{
	GLFunctionList* func = 0;
	GLFunctionsMap::const_iterator it = _map.find(name);
	if (it != _map.end())
		func = it->second;
	return func;
}
