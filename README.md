GLSL-Debugger
=============
[![Build Status](https://travis-ci.org/GLSL-Debugger/GLSL-Debugger.svg?branch=master)](https://travis-ci.org/GLSL-Debugger/GLSL-Debugger)

GLSL source level debugger.

This is the Open Source public release of the project originally known
as glslDevil ( http://www.vis.uni-stuttgart.de/glsldevil/ ), by
Thomas Klein, Magnus Strengert and Thomas Ertl.


Status and important notes
--------------------------

There is currently no active development and no active maintenance. 

Please expect problems and expect that you will need to touch some code in order to fix them. We are
around to help though. Historically, the source code became open source after being beginning its
life as part of a university project. After that, the community integrated CMake, made the project
build on Windows, current Linux systems, partially on OSX and integrated Mesa as GLSLCompiler. Most
of the development was done on Linux. Hence, Linux is probably the best choice in terms of debugger
functionality and build difficulties. 
If you experience any issues, please check the github issue tracker and file a
new ticket if need be.


Building
------

The only available build system is cmake. Example build steps:

```
mkdir Build && cd Build
cmake ..\
make
```

Cmake may generate files for big number of make utilities, please check the cmake
documentation for further information.

It is recommended to do out-of-source build.

Windows building requires additional dependencies installed:

- flex & bison (winflexbison is recommended: http://sourceforge.net/projects/winflexbison/
  gnu flex and bison might work too)
- Qt library
- glut for windows ( not tested, http://user.xmission.com/~nate/glut.html
	Installation instructions: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/#win)
  or freeglut ( tested, http://www.transmissionzero.co.uk/software/freeglut-devel/ )
- mhook required for windows build. It is added as submodule, use
  `git submodule update --init --recursive` if it was not checked with the tree.


On osx build was not tested, but cmake files may be used.


GLSL support
----------------

Old implementation of GLSLCompiler support GLSL up to 1.20. Support of newer versions is
avaliable by using mesa-based GLSLCompiler implementation. To enable it, cmake option
`USE_MESA` must be set. Use `-DUSE_MESA=ON` as argument to cmake command to do so.
Mesa currently support GLSL up to version 3.30, it will be extended in future.
Please note that mesa-based compiler is disabled by default yet and consider it as
alternative, that may have bugs. It will replace original compiler after some testing.

Also new compiler can be build with tests by passing `TESTS=ON` option to cmake.


Short-term goals
----------------

We need to test the build system and the source code more thoroughly. Hence, bugfixing and
testing should be a top priority for now.


Long-term goals
---------------

- [x] Replace Windows Detours dependency with ~apiTrace, glIntercept or EasyHook~. Replaced with mhook.
- [x] Check cmake-to-VS generation and fix it if needed.
- [ ] Support OSX.
- [x] Improve GLSL language grammar support to incorporate newer dialects including switch/case.
- [ ] Support OpenGL contexts for newer versions of OpenGL.
- [ ] Decouple the debugger logic from the UI (allow remote debugging)


Contribute
----------

We are looking for people who have an interest in this tool's capability to bring this project
back to life and move it forward, so get in touch, check out the code, try it out, fix some things
and push some changes!

Discussion is at glsl-debugger-development@googlegroups.com
https://groups.google.com/forum/?fromgroups#!forum/glsl-debugger-development

IRC channel at freenode: #glsl-debugger

