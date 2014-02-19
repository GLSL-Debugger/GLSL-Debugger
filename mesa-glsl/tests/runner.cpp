/*
 * locations.cpp
 *
 *  Created on: 05.01.2014
 */

#include "units/DebugVarTest.h"
#include "units/Compiler.h"
#include "glsldb/utils/dbgprint.h"
#include <cppunit/TestResult.h>

extern int _mesa_glsl_debug;


int main(int argc, char **argv)
{
	//_mesa_glsl_debug = 1;
	if (argc > 1)
		ShaderInput::setPath(std::string(argv[1]));
	setMaxDebugOutputLevel(DBGLVL_ALL);
	CppUnit::TestResult result;
	CompileShaderTest::suite()->run(&result);
	DebugVarTest::suite()->run(&result);

	return 0;
}
