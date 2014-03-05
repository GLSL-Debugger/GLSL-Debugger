/*
 * locations.cpp
 *
 *  Created on: 05.01.2014
 */

#include "units/Compiler.h"
#include "units/DebugVarTest.h"
#include "units/DebugChangeTest.h"
#include "units/DebugJumpTest.h"
#include "glsldb/utils/dbgprint.h"
#include <cppunit/TextTestRunner.h>

extern int _mesa_glsl_debug;

int main(int argc, char **argv)
{
	//_mesa_glsl_debug = 1;
	if (argc > 1)
		ShaderInput::setPath(std::string(argv[1]));
	setMaxDebugOutputLevel(DBGLVL_ERROR);
	CppUnit::TextTestRunner runner;
	runner.addTest(CompileShaderTest::suite());
	runner.addTest(DebugVarTest::suite());
	runner.addTest(DebugChangeTest::suite());
	runner.addTest(DebugJumpTest::suite());
	runner.run();
	return 0;
}
